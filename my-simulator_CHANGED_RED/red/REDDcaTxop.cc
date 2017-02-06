#include "ns3/assert.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/node.h"
#include "ns3/uinteger.h"
#include "ns3/pointer.h"

#include "REDDcaTxop.h"

#include "ns3/dcf-manager.h"
#include "ns3/mac-low.h"
#include "REDWifiMacQueue.h"
#include "ns3/mac-tx-middle.h"
#include "ns3/wifi-mac-trailer.h"
#include "ns3/wifi-mac.h"
#include "../../../ns-3.24/src/wifi/model/random-stream.h"

#undef NS_LOG_APPEND_CONTEXT
#define NS_LOG_APPEND_CONTEXT if (m_low != 0) { std::clog << "[mac=" << m_low->GetAddress () << "] "; }

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("REDDcaTxop");

class REDDcaTxop::Dcf : public DcfState
{
public:
  Dcf (REDDcaTxop * txop)
    : m_txop (txop)
  {
  }

private:
  virtual void DoNotifyAccessGranted (void)
  {
    m_txop->NotifyAccessGranted ();
  }
  virtual void DoNotifyInternalCollision (void)
  {
    m_txop->NotifyInternalCollision ();
  }
  virtual void DoNotifyCollision (void)
  {
    m_txop->NotifyCollision ();
  }
  virtual void DoNotifyChannelSwitching (void)
  {
    m_txop->NotifyChannelSwitching ();
  }
  virtual void DoNotifySleep (void)
  {
    m_txop->NotifySleep ();
  }
  virtual void DoNotifyWakeUp (void)
  {
    m_txop->NotifyWakeUp ();
  }

  REDDcaTxop *m_txop;
};


/**
 * Listener for MacLow events. Forwards to REDDcaTxop.
 */
class REDDcaTxop::TransmissionListener : public MacLowTransmissionListener
{
public:
  /**
   * Create a TransmissionListener for the given REDDcaTxop.
   *
   * \param txop
   */
  TransmissionListener (REDDcaTxop * txop)
    : MacLowTransmissionListener (),
      m_txop (txop)
  {
  }

  virtual ~TransmissionListener ()
  {
  }

  virtual void GotCts (double snr, WifiMode txMode)
  {
    m_txop->GotCts (snr, txMode);
  }
  virtual void MissedCts (void)
  {
    m_txop->MissedCts ();
  }
  virtual void GotAck (double snr, WifiMode txMode)
  {
    m_txop->GotAck (snr, txMode);
  }
  virtual void MissedAck (void)
  {
    m_txop->MissedAck ();
  }
  virtual void StartNext (void)
  {
    m_txop->StartNext ();
  }
  virtual void Cancel (void)
  {
    m_txop->Cancel ();
  }
  virtual void EndTxNoAck (void)
  {
    m_txop->EndTxNoAck ();
  }

private:
  REDDcaTxop *m_txop;
};

NS_OBJECT_ENSURE_REGISTERED (REDDcaTxop);

TypeId
REDDcaTxop::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::REDDcaTxop")
    .SetParent<ns3::Dcf> ()
    .SetGroupName ("Wifi")
    .AddConstructor<REDDcaTxop> ()
    .AddAttribute ("Queue", "The REDWifiMacQueue object",
                   PointerValue (),
                   MakePointerAccessor (&REDDcaTxop::GetQueue),
                   MakePointerChecker<REDWifiMacQueue> ())
  ;
  return tid;
}

REDDcaTxop::REDDcaTxop ()
  : m_manager (0),
    m_currentPacket (0)
{
  NS_LOG_FUNCTION (this);
  m_transmissionListener = new REDDcaTxop::TransmissionListener (this);
  m_dcf = new REDDcaTxop::Dcf (this);
  m_queue = CreateObject<RED_MainBuff> ();
  m_rng = new RealRandomStream ();
}

REDDcaTxop::~REDDcaTxop ()
{
  NS_LOG_FUNCTION (this);
}

void
REDDcaTxop::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_queue = 0;
  m_low = 0;
  m_stationManager = 0;
  delete m_transmissionListener;
  delete m_dcf;
  delete m_rng;
  m_transmissionListener = 0;
  m_dcf = 0;
  m_rng = 0;
  m_txMiddle = 0;
}

void
REDDcaTxop::SetManager (DcfManager *manager)
{
  NS_LOG_FUNCTION (this << manager);
  m_manager = manager;
  m_manager->Add (m_dcf);
}

void REDDcaTxop::SetTxMiddle (MacTxMiddle *txMiddle)
{
  m_txMiddle = txMiddle;
}

void
REDDcaTxop::SetLow (Ptr<MacLow> low)
{
  NS_LOG_FUNCTION (this << low);
  m_low = low;
}

void
REDDcaTxop::SetWifiRemoteStationManager (Ptr<WifiRemoteStationManager> remoteManager)
{
  NS_LOG_FUNCTION (this << remoteManager);
  m_stationManager = remoteManager;
}

void
REDDcaTxop::SetTxOkCallback (TxOk callback)
{
  NS_LOG_FUNCTION (this << &callback);
  m_txOkCallback = callback;
}

void
REDDcaTxop::SetTxFailedCallback (TxFailed callback)
{
  NS_LOG_FUNCTION (this << &callback);
  m_txFailedCallback = callback;
}

Ptr<REDWifiMacQueue >
REDDcaTxop::GetQueue () const
{
  NS_LOG_FUNCTION (this);
  return m_queue;
}

void
REDDcaTxop::SetMinCw (uint32_t minCw)
{
  NS_LOG_FUNCTION (this << minCw);
  m_dcf->SetCwMin (minCw);
}

void
REDDcaTxop::SetMaxCw (uint32_t maxCw)
{
  NS_LOG_FUNCTION (this << maxCw);
  m_dcf->SetCwMax (maxCw);
}

void
REDDcaTxop::SetAifsn (uint32_t aifsn)
{
  NS_LOG_FUNCTION (this << aifsn);
  m_dcf->SetAifsn (aifsn);
}

uint32_t
REDDcaTxop::GetMinCw (void) const
{
  NS_LOG_FUNCTION (this);
  return m_dcf->GetCwMin ();
}

uint32_t
REDDcaTxop::GetMaxCw (void) const
{
  NS_LOG_FUNCTION (this);
  return m_dcf->GetCwMax ();
}

uint32_t
REDDcaTxop::GetAifsn (void) const
{
  NS_LOG_FUNCTION (this);
  return m_dcf->GetAifsn ();
}

void
REDDcaTxop::Queue (Ptr<const Packet> packet, const WifiMacHeader &hdr)
{
  NS_LOG_FUNCTION (this << packet << &hdr);
  WifiMacTrailer fcs;
  uint32_t fullPacketSize = hdr.GetSerializedSize () + packet->GetSize () + fcs.GetSerializedSize ();
  m_stationManager->PrepareForQueue (hdr.GetAddr1 (), &hdr,
                                     packet, fullPacketSize);
  m_queue->Enqueue (packet, hdr);
  StartAccessIfNeeded ();
}

int64_t
REDDcaTxop::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_rng->AssignStreams (stream);
  return 1;
}

void
REDDcaTxop::RestartAccessIfNeeded (void)
{
  NS_LOG_FUNCTION (this);
  if ((m_currentPacket != 0
       || !m_queue->IsEmpty ())
      && !m_dcf->IsAccessRequested ())
    {
      m_manager->RequestAccess (m_dcf);
    }
}

void
REDDcaTxop::StartAccessIfNeeded (void)
{
  NS_LOG_FUNCTION (this);
  if (m_currentPacket == 0
      && !m_queue->IsEmpty ()
      && !m_dcf->IsAccessRequested ())
    {
      m_manager->RequestAccess (m_dcf);
    }
}

Ptr<MacLow>
REDDcaTxop::Low (void)
{
  NS_LOG_FUNCTION (this);
  return m_low;
}

bool
REDDcaTxop::NeedRts (Ptr<const Packet> packet, const WifiMacHeader *header)
{
  NS_LOG_FUNCTION (this << packet << header);
  return m_stationManager->NeedRts (header->GetAddr1 (), header,
                                    packet);
}

void
REDDcaTxop::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  m_dcf->ResetCw ();
  m_dcf->StartBackoffNow (m_rng->GetNext (0, m_dcf->GetCw ()));
  ns3::Dcf::DoInitialize ();
}

bool
REDDcaTxop::NeedRtsRetransmission (void)
{
  NS_LOG_FUNCTION (this);
  return m_stationManager->NeedRtsRetransmission (m_currentHdr.GetAddr1 (), &m_currentHdr,
                                                  m_currentPacket);
}

bool
REDDcaTxop::NeedDataRetransmission (void)
{
  NS_LOG_FUNCTION (this);
  return m_stationManager->NeedDataRetransmission (m_currentHdr.GetAddr1 (), &m_currentHdr,
                                                   m_currentPacket);
}

bool
REDDcaTxop::NeedFragmentation (void)
{
  NS_LOG_FUNCTION (this);
  return m_stationManager->NeedFragmentation (m_currentHdr.GetAddr1 (), &m_currentHdr,
                                              m_currentPacket);
}

void
REDDcaTxop::NextFragment (void)
{
  NS_LOG_FUNCTION (this);
  m_fragmentNumber++;
}

uint32_t
REDDcaTxop::GetFragmentSize (void)
{
  NS_LOG_FUNCTION (this);
  return m_stationManager->GetFragmentSize (m_currentHdr.GetAddr1 (), &m_currentHdr,
                                            m_currentPacket, m_fragmentNumber);
}

bool
REDDcaTxop::IsLastFragment (void)
{
  NS_LOG_FUNCTION (this);
  return m_stationManager->IsLastFragment (m_currentHdr.GetAddr1 (), &m_currentHdr,
                                           m_currentPacket, m_fragmentNumber);
}

uint32_t
REDDcaTxop::GetNextFragmentSize (void)
{
  NS_LOG_FUNCTION (this);
  return m_stationManager->GetFragmentSize (m_currentHdr.GetAddr1 (), &m_currentHdr,
                                            m_currentPacket, m_fragmentNumber + 1);
}

uint32_t
REDDcaTxop::GetFragmentOffset (void)
{
  NS_LOG_FUNCTION (this);
  return m_stationManager->GetFragmentOffset (m_currentHdr.GetAddr1 (), &m_currentHdr,
                                              m_currentPacket, m_fragmentNumber);
}

Ptr<Packet>
REDDcaTxop::GetFragmentPacket (WifiMacHeader *hdr)
{
  NS_LOG_FUNCTION (this << hdr);
  *hdr = m_currentHdr;
  hdr->SetFragmentNumber (m_fragmentNumber);
  uint32_t startOffset = GetFragmentOffset ();
  Ptr<Packet> fragment;
  if (IsLastFragment ())
    {
      hdr->SetNoMoreFragments ();
    }
  else
    {
      hdr->SetMoreFragments ();
    }
  fragment = m_currentPacket->CreateFragment (startOffset,
                                              GetFragmentSize ());
  return fragment;
}

bool
REDDcaTxop::NeedsAccess (void) const
{
  NS_LOG_FUNCTION (this);
  return !m_queue->IsEmpty () || m_currentPacket != 0;
}
void
REDDcaTxop::NotifyAccessGranted (void)
{
  NS_LOG_FUNCTION (this);
  if (m_currentPacket == 0)
    {
      if (m_queue->IsEmpty ())
        {
          NS_LOG_DEBUG ("queue empty");
          return;
        }
      m_currentPacket = m_queue->Dequeue (&m_currentHdr);
      NS_ASSERT (m_currentPacket != 0);
      uint16_t sequence = m_txMiddle->GetNextSequenceNumberfor (&m_currentHdr);
      m_currentHdr.SetSequenceNumber (sequence);
      m_stationManager->UpdateFragmentationThreshold ();
      m_currentHdr.SetFragmentNumber (0);
      m_currentHdr.SetNoMoreFragments ();
      m_currentHdr.SetNoRetry ();
      m_fragmentNumber = 0;
      NS_LOG_DEBUG ("dequeued size=" << m_currentPacket->GetSize () <<
                    ", to=" << m_currentHdr.GetAddr1 () <<
                    ", seq=" << m_currentHdr.GetSequenceControl ());
    }
  MacLowTransmissionParameters params;
  params.DisableOverrideDurationId ();
  if (m_currentHdr.GetAddr1 ().IsGroup ())
    {
      params.DisableRts ();
      params.DisableAck ();
      params.DisableNextData ();
      Low ()->StartTransmission (m_currentPacket,
                                 &m_currentHdr,
                                 params,
                                 m_transmissionListener);
      NS_LOG_DEBUG ("tx broadcast");
    }
  else
    {
      params.EnableAck ();

      if (NeedFragmentation ())
        {
          WifiMacHeader hdr;
          Ptr<Packet> fragment = GetFragmentPacket (&hdr);
          if (NeedRts (fragment, &hdr))
            {
              params.EnableRts ();
            }
          else
            {
              params.DisableRts ();
            }
          if (IsLastFragment ())
            {
              NS_LOG_DEBUG ("fragmenting last fragment size=" << fragment->GetSize ());
              params.DisableNextData ();
            }
          else
            {
              NS_LOG_DEBUG ("fragmenting size=" << fragment->GetSize ());
              params.EnableNextData (GetNextFragmentSize ());
            }
          Low ()->StartTransmission (fragment, &hdr, params,
                                     m_transmissionListener);
        }
      else
        {
          if (NeedRts (m_currentPacket, &m_currentHdr))
            {
              params.EnableRts ();
              NS_LOG_DEBUG ("tx unicast rts");
            }
          else
            {
              params.DisableRts ();
              NS_LOG_DEBUG ("tx unicast");
            }
          params.DisableNextData ();
          Low ()->StartTransmission (m_currentPacket, &m_currentHdr,
                                     params, m_transmissionListener);
        }
    }
}

void
REDDcaTxop::NotifyInternalCollision (void)
{
  NS_LOG_FUNCTION (this);
  NotifyCollision ();
}

void
REDDcaTxop::NotifyCollision (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("collision");
  m_dcf->StartBackoffNow (m_rng->GetNext (0, m_dcf->GetCw ()));
  RestartAccessIfNeeded ();
}

void
REDDcaTxop::NotifyChannelSwitching (void)
{
  NS_LOG_FUNCTION (this);
  m_queue->Flush ();
  m_currentPacket = 0;
}

void
REDDcaTxop::NotifySleep (void)
{
  NS_LOG_FUNCTION (this);
  if (m_currentPacket != 0)
    {
      m_queue->PushFront (m_currentPacket, m_currentHdr);
      m_currentPacket = 0;
    }
}

void
REDDcaTxop::NotifyWakeUp (void)
{
  NS_LOG_FUNCTION (this);
  RestartAccessIfNeeded ();
}

void
REDDcaTxop::GotCts (double snr, WifiMode txMode)
{
  NS_LOG_FUNCTION (this << snr << txMode);
  NS_LOG_DEBUG ("got cts");
}

void
REDDcaTxop::MissedCts (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("missed cts");
  if (!NeedRtsRetransmission ())
    {
      NS_LOG_DEBUG ("Cts Fail");
      m_stationManager->ReportFinalRtsFailed (m_currentHdr.GetAddr1 (), &m_currentHdr);
      if (!m_txFailedCallback.IsNull ())
        {
          m_txFailedCallback (m_currentHdr);
        }
      //to reset the dcf.
      m_currentPacket = 0;
      m_dcf->ResetCw ();
    }
  else
    {
      m_dcf->UpdateFailedCw ();
    }
  m_dcf->StartBackoffNow (m_rng->GetNext (0, m_dcf->GetCw ()));
  RestartAccessIfNeeded ();
}

void
REDDcaTxop::GotAck (double snr, WifiMode txMode)
{
  NS_LOG_FUNCTION (this << snr << txMode);
  if (!NeedFragmentation ()
      || IsLastFragment ())
    {
      NS_LOG_DEBUG ("got ack. tx done.");
      if (!m_txOkCallback.IsNull ())
        {
          m_txOkCallback (m_currentHdr);
        }

      /* we are not fragmenting or we are done fragmenting
       * so we can get rid of that packet now.
       */
      m_currentPacket = 0;
      m_dcf->ResetCw ();
      m_dcf->StartBackoffNow (m_rng->GetNext (0, m_dcf->GetCw ()));
      RestartAccessIfNeeded ();
    }
  else
    {
      NS_LOG_DEBUG ("got ack. tx not done, size=" << m_currentPacket->GetSize ());
    }
}

void
REDDcaTxop::MissedAck (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("missed ack");
  if (!NeedDataRetransmission ())
    {
      NS_LOG_DEBUG ("Ack Fail");
      m_stationManager->ReportFinalDataFailed (m_currentHdr.GetAddr1 (), &m_currentHdr);
      if (!m_txFailedCallback.IsNull ())
        {
          m_txFailedCallback (m_currentHdr);
        }
      //to reset the dcf.
      m_currentPacket = 0;
      m_dcf->ResetCw ();
    }
  else
    {
      NS_LOG_DEBUG ("Retransmit");
      m_currentHdr.SetRetry ();
      m_dcf->UpdateFailedCw ();
    }
  m_dcf->StartBackoffNow (m_rng->GetNext (0, m_dcf->GetCw ()));
  RestartAccessIfNeeded ();
}

void
REDDcaTxop::StartNext (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("start next packet fragment");
  /* this callback is used only for fragments. */
  NextFragment ();
  WifiMacHeader hdr;
  Ptr<Packet> fragment = GetFragmentPacket (&hdr);
  MacLowTransmissionParameters params;
  params.EnableAck ();
  params.DisableRts ();
  params.DisableOverrideDurationId ();
  if (IsLastFragment ())
    {
      params.DisableNextData ();
    }
  else
    {
      params.EnableNextData (GetNextFragmentSize ());
    }
  Low ()->StartTransmission (fragment, &hdr, params, m_transmissionListener);
}

void
REDDcaTxop::Cancel (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("transmission cancelled");
  /**
   * This happens in only one case: in an AP, you have two REDDcaTxop:
   *   - one is used exclusively for beacons and has a high priority.
   *   - the other is used for everything else and has a normal
   *     priority.
   *
   * If the normal queue tries to send a unicast data frame, but
   * if the tx fails (ack timeout), it starts a backoff. If the beacon
   * queue gets a tx oportunity during this backoff, it will trigger
   * a call to this Cancel function.
   *
   * Since we are already doing a backoff, we will get access to
   * the medium when we can, we have nothing to do here. We just
   * ignore the cancel event and wait until we are given again a
   * tx oportunity.
   *
   * Note that this is really non-trivial because each of these
   * frames is assigned a sequence number from the same sequence
   * counter (because this is a non-802.11e device) so, the scheme
   * described here fails to ensure in-order delivery of frames
   * at the receiving side. This, however, does not matter in
   * this case because we assume that the receiving side does not
   * update its <seq,ad> tupple for packets whose destination
   * address is a broadcast address.
   */
}

void
REDDcaTxop::EndTxNoAck (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("a transmission that did not require an ACK just finished");
  m_currentPacket = 0;
  m_dcf->ResetCw ();
  m_dcf->StartBackoffNow (m_rng->GetNext (0, m_dcf->GetCw ()));
  StartAccessIfNeeded ();
}

} //namespace ns3