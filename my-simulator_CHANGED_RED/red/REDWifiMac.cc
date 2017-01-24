#include "REDWifiMac.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/mac-rx-middle.h"
#include "ns3/mac-tx-middle.h"
#include "ns3/mac-low.h"
#include "ns3/dcf.h"
#include "ns3/dcf-manager.h"
#include "ns3/wifi-phy.h"
#include "ns3/msdu-aggregator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("REDWifiMac");

NS_OBJECT_ENSURE_REGISTERED (REDWifiMac);

REDWifiMac::REDWifiMac ()
{
  NS_LOG_FUNCTION (this);
  m_rxMiddle = new MacRxMiddle ();
  m_rxMiddle->SetForwardCallback (MakeCallback (&REDWifiMac::Receive, this));

  m_txMiddle = new MacTxMiddle ();

  m_low = CreateObject<MacLow> ();
  m_low->SetRxCallback (MakeCallback (&MacRxMiddle::Receive, m_rxMiddle));

  m_dcfManager = new DcfManager ();
  m_dcfManager->SetupLowListener (m_low);

  m_dca = CreateObject<DcaTxop> ();
  m_dca->SetLow (m_low);
  m_dca->SetManager (m_dcfManager);
  m_dca->SetTxMiddle (m_txMiddle);
  m_dca->SetTxOkCallback (MakeCallback (&REDWifiMac::TxOk, this));
  m_dca->SetTxFailedCallback (MakeCallback (&REDWifiMac::TxFailed, this));

  //Construct the EDCAFs. The ordering is important - highest
  //priority (Table 9-1 UP-to-AC mapping; IEEE 802.11-2012) must be created
  //first.
  SetupEdcaQueue (AC_VO);
  SetupEdcaQueue (AC_VI);
  SetupEdcaQueue (AC_BE);
  SetupEdcaQueue (AC_BK);

  SetTypeOfStation (ADHOC_STA);
}

REDWifiMac::~REDWifiMac ()
{
  NS_LOG_FUNCTION (this);
}

void
REDWifiMac::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  m_dca->Initialize ();

  for (EdcaQueues::iterator i = m_edca.begin (); i != m_edca.end (); ++i)
    {
      i->second->Initialize ();
    }
}

void
REDWifiMac::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_rxMiddle;
  m_rxMiddle = 0;

  delete m_txMiddle;
  m_txMiddle = 0;

  delete m_dcfManager;
  m_dcfManager = 0;

  m_low->Dispose ();
  m_low = 0;

  m_phy = 0;
  m_stationManager = 0;

  m_dca->Dispose ();
  m_dca = 0;

  for (EdcaQueues::iterator i = m_edca.begin (); i != m_edca.end (); ++i)
    {
      i->second = 0;
    }
}

void
REDWifiMac::SetWifiRemoteStationManager (Ptr<WifiRemoteStationManager> stationManager)
{
  NS_LOG_FUNCTION (this << stationManager);
  m_stationManager = stationManager;
  m_stationManager->SetHtSupported (GetHtSupported ());
  m_stationManager->SetVhtSupported (GetVhtSupported ());
  m_low->SetWifiRemoteStationManager (stationManager);

  m_dca->SetWifiRemoteStationManager (stationManager);

  for (EdcaQueues::iterator i = m_edca.begin (); i != m_edca.end (); ++i)
    {
      i->second->SetWifiRemoteStationManager (stationManager);
    }
}

Ptr<WifiRemoteStationManager>
REDWifiMac::GetWifiRemoteStationManager () const
{
  return m_stationManager;
}

void
REDWifiMac::SetupEdcaQueue (enum AcIndex ac)
{
  NS_LOG_FUNCTION (this << ac);

  //Our caller shouldn't be attempting to setup a queue that is
  //already configured.
  NS_ASSERT (m_edca.find (ac) == m_edca.end ());

  Ptr<EdcaTxopN> edca = CreateObject<EdcaTxopN> ();
  edca->SetLow (m_low);
  edca->SetManager (m_dcfManager);
  edca->SetTxMiddle (m_txMiddle);
  edca->SetTxOkCallback (MakeCallback (&REDWifiMac::TxOk, this));
  edca->SetTxFailedCallback (MakeCallback (&REDWifiMac::TxFailed, this));
  edca->SetAccessCategory (ac);
  edca->CompleteConfig ();
  m_edca.insert (std::make_pair (ac, edca));
}

void
REDWifiMac::SetTypeOfStation (TypeOfStation type)
{
  NS_LOG_FUNCTION (this << type);
  for (EdcaQueues::iterator i = m_edca.begin (); i != m_edca.end (); ++i)
    {
      i->second->SetTypeOfStation (type);
    }
}

Ptr<DcaTxop>
REDWifiMac::GetDcaTxop () const
{
  return m_dca;
}

Ptr<EdcaTxopN>
REDWifiMac::GetVOQueue () const
{
  return m_edca.find (AC_VO)->second;
}

Ptr<EdcaTxopN>
REDWifiMac::GetVIQueue () const
{
  return m_edca.find (AC_VI)->second;
}

Ptr<EdcaTxopN>
REDWifiMac::GetBEQueue () const
{
  return m_edca.find (AC_BE)->second;
}

Ptr<EdcaTxopN>
REDWifiMac::GetBKQueue () const
{
  return m_edca.find (AC_BK)->second;
}

void
REDWifiMac::SetWifiPhy (Ptr<WifiPhy> phy)
{
  NS_LOG_FUNCTION (this << phy);
  m_phy = phy;
  m_dcfManager->SetupPhyListener (phy);
  m_low->SetPhy (phy);
}

Ptr<WifiPhy>
REDWifiMac::GetWifiPhy (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phy;
}

void
REDWifiMac::ResetWifiPhy (void)
{
  NS_LOG_FUNCTION (this);
  m_low->ResetPhy ();
  m_dcfManager->RemovePhyListener (m_phy);
  m_phy = 0;
}

void
REDWifiMac::SetForwardUpCallback (ForwardUpCallback upCallback)
{
  NS_LOG_FUNCTION (this);
  m_forwardUp = upCallback;
}

void
REDWifiMac::SetLinkUpCallback (Callback<void> linkUp)
{
  NS_LOG_FUNCTION (this << &linkUp);
  m_linkUp = linkUp;
  linkUp ();
}

void
REDWifiMac::SetLinkDownCallback (Callback<void> linkDown)
{
  NS_LOG_FUNCTION (this);
  m_linkDown = linkDown;
}

void
REDWifiMac::SetQosSupported (bool enable)
{
  NS_LOG_FUNCTION (this);
  m_qosSupported = enable;
}

bool
REDWifiMac::GetQosSupported () const
{
  return m_qosSupported;
}

void
REDWifiMac::SetHtSupported (bool enable)
{
  NS_LOG_FUNCTION (this);
  m_htSupported = enable;
}

bool
REDWifiMac::GetVhtSupported () const
{
  return m_vhtSupported;
}

void
REDWifiMac::SetVhtSupported (bool enable)
{
  NS_LOG_FUNCTION (this);
  m_vhtSupported = enable;
}

bool
REDWifiMac::GetHtSupported () const
{
  return m_htSupported;
}

void
REDWifiMac::SetCtsToSelfSupported (bool enable)
{
  NS_LOG_FUNCTION (this);
  m_low->SetCtsToSelfSupported (enable);
}

bool
REDWifiMac::GetCtsToSelfSupported () const
{
  return m_low->GetCtsToSelfSupported ();
}

void
REDWifiMac::SetSlot (Time slotTime)
{
  NS_LOG_FUNCTION (this << slotTime);
  m_dcfManager->SetSlot (slotTime);
  m_low->SetSlotTime (slotTime);
}

Time
REDWifiMac::GetSlot (void) const
{
  return m_low->GetSlotTime ();
}

void
REDWifiMac::SetSifs (Time sifs)
{
  NS_LOG_FUNCTION (this << sifs);
  m_dcfManager->SetSifs (sifs);
  m_low->SetSifs (sifs);
}

Time
REDWifiMac::GetSifs (void) const
{
  return m_low->GetSifs ();
}

void
REDWifiMac::SetEifsNoDifs (Time eifsNoDifs)
{
  NS_LOG_FUNCTION (this << eifsNoDifs);
  m_dcfManager->SetEifsNoDifs (eifsNoDifs);
}

Time
REDWifiMac::GetEifsNoDifs (void) const
{
  return m_dcfManager->GetEifsNoDifs ();
}

void
REDWifiMac::SetRifs (Time rifs)
{
  NS_LOG_FUNCTION (this << rifs);
  m_low->SetRifs (rifs);
}

Time
REDWifiMac::GetRifs (void) const
{
  return m_low->GetRifs ();
}

void
REDWifiMac::SetPifs (Time pifs)
{
  NS_LOG_FUNCTION (this << pifs);
  m_low->SetPifs (pifs);
}

Time
REDWifiMac::GetPifs (void) const
{
  return m_low->GetPifs ();
}

void
REDWifiMac::SetAckTimeout (Time ackTimeout)
{
  NS_LOG_FUNCTION (this << ackTimeout);
  m_low->SetAckTimeout (ackTimeout);
}

Time
REDWifiMac::GetAckTimeout (void) const
{
  return m_low->GetAckTimeout ();
}

void
REDWifiMac::SetCtsTimeout (Time ctsTimeout)
{
  NS_LOG_FUNCTION (this << ctsTimeout);
  m_low->SetCtsTimeout (ctsTimeout);
}

Time
REDWifiMac::GetCtsTimeout (void) const
{
  return m_low->GetCtsTimeout ();
}

void
REDWifiMac::SetBasicBlockAckTimeout (Time blockAckTimeout)
{
  NS_LOG_FUNCTION (this << blockAckTimeout);
  m_low->SetBasicBlockAckTimeout (blockAckTimeout);
}

Time
REDWifiMac::GetBasicBlockAckTimeout (void) const
{
  return m_low->GetBasicBlockAckTimeout ();
}

void
REDWifiMac::SetCompressedBlockAckTimeout (Time blockAckTimeout)
{
  NS_LOG_FUNCTION (this << blockAckTimeout);
  m_low->SetCompressedBlockAckTimeout (blockAckTimeout);
}

Time
REDWifiMac::GetCompressedBlockAckTimeout (void) const
{
  return m_low->GetCompressedBlockAckTimeout ();
}

void
REDWifiMac::SetAddress (Mac48Address address)
{
  NS_LOG_FUNCTION (this << address);
  m_low->SetAddress (address);
  SetBssid(address);
}

Mac48Address
REDWifiMac::GetAddress (void) const
{
  return m_low->GetAddress ();
}

void
REDWifiMac::SetSsid (Ssid ssid)
{
  NS_LOG_FUNCTION (this << ssid);
  m_ssid = ssid;
}

Ssid
REDWifiMac::GetSsid (void) const
{
  return m_ssid;
}

void
REDWifiMac::SetBssid (Mac48Address bssid)
{
  NS_LOG_FUNCTION (this << bssid);
  m_low->SetBssid (bssid);
}

Mac48Address
REDWifiMac::GetBssid (void) const
{
  return m_low->GetBssid ();
}

void
REDWifiMac::SetPromisc (void)
{
  m_low->SetPromisc ();
}

void
REDWifiMac::Enqueue (Ptr<const Packet> packet,
                         Mac48Address to, Mac48Address from)
{
  //We expect REDWifiMac subclasses which do support forwarding (e.g.,
  //AP) to override this method. Therefore, we throw a fatal error if
  //someone tries to invoke this method on a class which has not done
  //this.
  NS_FATAL_ERROR ("This MAC entity (" << this << ", " << GetAddress ()
                                      << ") does not support Enqueue() with from address");
}

void
REDWifiMac::Enqueue (Ptr<const Packet> packet,
                         Mac48Address to)
{
    NS_LOG_FUNCTION (this << packet << to);
  if (m_stationManager->IsBrandNew (to))
    {
      m_stationManager->AddAllSupportedModes (to);
      m_stationManager->RecordDisassociated (to);
    }

  WifiMacHeader hdr;

  //If we are not a QoS STA then we definitely want to use AC_BE to
  //transmit the packet. A TID of zero will map to AC_BE (through \c
  //QosUtilsMapTidToAc()), so we use that as our default here.

  //For now, a STA that supports QoS does not support non-QoS
  //associations, and vice versa. In future the STA model should fall
  //back to non-QoS if talking to a peer that is also non-QoS. At
  //that point there will need to be per-station QoS state maintained
  //by the association state machine, and consulted here.
  hdr.SetTypeData ();

  hdr.SetAddr1 (to);
  hdr.SetAddr2 (m_low->GetAddress ());
  hdr.SetAddr3 (GetBssid ());
  hdr.SetDsNotFrom ();
  hdr.SetDsNotTo ();

  m_dca->Queue (packet, hdr);
}

bool
REDWifiMac::SupportsSendFrom (void) const
{
  return false;
}

void
REDWifiMac::ForwardUp (Ptr<Packet> packet, Mac48Address from, Mac48Address to)
{
  NS_LOG_FUNCTION (this << packet << from);
  m_forwardUp (packet, from, to);
}

void
REDWifiMac::Receive (Ptr<Packet> packet, const WifiMacHeader *hdr)
{
  NS_LOG_FUNCTION (this << packet << hdr);


  NS_ASSERT (!hdr->IsCtl ());

  Mac48Address to = hdr->GetAddr1 ();
  Mac48Address from = hdr->GetAddr2 ();

  if (hdr->IsData ())
    {
      ForwardUp (packet, from, to);
      return;
    }


  //We don't know how to deal with any frame that is not addressed to
  //us (and odds are there is nothing sensible we could do anyway),
  //so we ignore such frames.
  //
  //The derived class may also do some such filtering, but it doesn't
  //hurt to have it here too as a backstop.
  if (to != GetAddress ())
    {
      return;
    }

  if (hdr->IsMgt () && hdr->IsAction ())
    {
      //There is currently only any reason for Management Action
      //frames to be flying about if we are a QoS STA.
      NS_ASSERT (m_qosSupported);

      WifiActionHeader actionHdr;
      packet->RemoveHeader (actionHdr);

      switch (actionHdr.GetCategory ())
        {
        case WifiActionHeader::BLOCK_ACK:

          switch (actionHdr.GetAction ().blockAck)
            {
            case WifiActionHeader::BLOCK_ACK_ADDBA_REQUEST:
              {
                MgtAddBaRequestHeader reqHdr;
                packet->RemoveHeader (reqHdr);

                //We've received an ADDBA Request. Our policy here is
                //to automatically accept it, so we get the ADDBA
                //Response on it's way immediately.
                SendAddBaResponse (&reqHdr, from);
                //This frame is now completely dealt with, so we're done.
                return;
              }
            case WifiActionHeader::BLOCK_ACK_ADDBA_RESPONSE:
              {
                MgtAddBaResponseHeader respHdr;
                packet->RemoveHeader (respHdr);

                //We've received an ADDBA Response. We assume that it
                //indicates success after an ADDBA Request we have
                //sent (we could, in principle, check this, but it
                //seems a waste given the level of the current model)
                //and act by locally establishing the agreement on
                //the appropriate queue.
                AcIndex ac = QosUtilsMapTidToAc (respHdr.GetTid ());
                m_edca[ac]->GotAddBaResponse (&respHdr, from);
                //This frame is now completely dealt with, so we're done.
                return;
              }
            case WifiActionHeader::BLOCK_ACK_DELBA:
              {
                MgtDelBaHeader delBaHdr;
                packet->RemoveHeader (delBaHdr);

                if (delBaHdr.IsByOriginator ())
                  {
                    //This DELBA frame was sent by the originator, so
                    //this means that an ingoing established
                    //agreement exists in MacLow and we need to
                    //destroy it.
                    m_low->DestroyBlockAckAgreement (from, delBaHdr.GetTid ());
                  }
                else
                  {
                    //We must have been the originator. We need to
                    //tell the correct queue that the agreement has
                    //been torn down
                    AcIndex ac = QosUtilsMapTidToAc (delBaHdr.GetTid ());
                    m_edca[ac]->GotDelBaFrame (&delBaHdr, from);
                  }
                //This frame is now completely dealt with, so we're done.
                return;
              }
            default:
              NS_FATAL_ERROR ("Unsupported Action field in Block Ack Action frame");
              return;
            }
        default:
          NS_FATAL_ERROR ("Unsupported Action frame received");
          return;
        }
    }
  NS_FATAL_ERROR ("Don't know how to handle frame (type=" << hdr->GetType ());
}

void
REDWifiMac::DeaggregateAmsduAndForward (Ptr<Packet> aggregatedPacket,
                                            const WifiMacHeader *hdr)
{
  MsduAggregator::DeaggregatedMsdus packets =
    MsduAggregator::Deaggregate (aggregatedPacket);

  for (MsduAggregator::DeaggregatedMsdusCI i = packets.begin ();
       i != packets.end (); ++i)
    {
      ForwardUp ((*i).first, (*i).second.GetSourceAddr (),
                 (*i).second.GetDestinationAddr ());
    }
}

void
REDWifiMac::SendAddBaResponse (const MgtAddBaRequestHeader *reqHdr,
                                   Mac48Address originator)
{
  NS_LOG_FUNCTION (this);
  WifiMacHeader hdr;
  hdr.SetAction ();
  hdr.SetAddr1 (originator);
  hdr.SetAddr2 (GetAddress ());
  hdr.SetAddr3 (GetAddress ());
  hdr.SetDsNotFrom ();
  hdr.SetDsNotTo ();

  MgtAddBaResponseHeader respHdr;
  StatusCode code;
  code.SetSuccess ();
  respHdr.SetStatusCode (code);
  //Here a control about queues type?
  respHdr.SetAmsduSupport (reqHdr->IsAmsduSupported ());

  if (reqHdr->IsImmediateBlockAck ())
    {
      respHdr.SetImmediateBlockAck ();
    }
  else
    {
      respHdr.SetDelayedBlockAck ();
    }
  respHdr.SetTid (reqHdr->GetTid ());
  //For now there's not no control about limit of reception. We
  //assume that receiver has no limit on reception. However we assume
  //that a receiver sets a bufferSize in order to satisfy next
  //equation: (bufferSize + 1) % 16 = 0 So if a recipient is able to
  //buffer a packet, it should be also able to buffer all possible
  //packet's fragments. See section 7.3.1.14 in IEEE802.11e for more details.
  respHdr.SetBufferSize (1023);
  respHdr.SetTimeout (reqHdr->GetTimeout ());

  WifiActionHeader actionHdr;
  WifiActionHeader::ActionValue action;
  action.blockAck = WifiActionHeader::BLOCK_ACK_ADDBA_RESPONSE;
  actionHdr.SetAction (WifiActionHeader::BLOCK_ACK, action);

  Ptr<Packet> packet = Create<Packet> ();
  packet->AddHeader (respHdr);
  packet->AddHeader (actionHdr);

  //We need to notify our MacLow object as it will have to buffer all
  //correctly received packets for this Block Ack session
  m_low->CreateBlockAckAgreement (&respHdr, originator,
                                  reqHdr->GetStartingSequence ());

  //It is unclear which queue this frame should go into. For now we
  //bung it into the queue corresponding to the TID for which we are
  //establishing an agreement, and push it to the head.
  m_edca[QosUtilsMapTidToAc (reqHdr->GetTid ())]->PushFront (packet, hdr);
}

TypeId
REDWifiMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::REDWifiMac")
    .SetParent<WifiMac> ()
    .SetGroupName ("Wifi")
    .AddAttribute ("QosSupported",
                   "This Boolean attribute is set to enable 802.11e/WMM-style QoS support at this STA",
                   BooleanValue (false),
                   MakeBooleanAccessor (&REDWifiMac::SetQosSupported,
                                        &REDWifiMac::GetQosSupported),
                   MakeBooleanChecker ())
    .AddAttribute ("HtSupported",
                   "This Boolean attribute is set to enable 802.11n support at this STA",
                   BooleanValue (false),
                   MakeBooleanAccessor (&REDWifiMac::SetHtSupported,
                                        &REDWifiMac::GetHtSupported),
                   MakeBooleanChecker ())
    .AddAttribute ("VhtSupported",
                   "This Boolean attribute is set to enable 802.11ac support at this STA",
                   BooleanValue (false),
                   MakeBooleanAccessor (&REDWifiMac::SetVhtSupported,
                                        &REDWifiMac::GetVhtSupported),
                   MakeBooleanChecker ())
    .AddAttribute ("CtsToSelfSupported",
                   "Use CTS to Self when using a rate that is not in the basic set rate",
                   BooleanValue (false),
                   MakeBooleanAccessor (&REDWifiMac::SetCtsToSelfSupported,
                                        &REDWifiMac::GetCtsToSelfSupported),
                   MakeBooleanChecker ())
    .AddAttribute ("DcaTxop", "The DcaTxop object",
                   PointerValue (),
                   MakePointerAccessor (&REDWifiMac::GetDcaTxop),
                   MakePointerChecker<DcaTxop> ())
    .AddAttribute ("VO_EdcaTxopN",
                   "Queue that manages packets belonging to AC_VO access class",
                   PointerValue (),
                   MakePointerAccessor (&REDWifiMac::GetVOQueue),
                   MakePointerChecker<EdcaTxopN> ())
    .AddAttribute ("VI_EdcaTxopN",
                   "Queue that manages packets belonging to AC_VI access class",
                   PointerValue (),
                   MakePointerAccessor (&REDWifiMac::GetVIQueue),
                   MakePointerChecker<EdcaTxopN> ())
    .AddAttribute ("BE_EdcaTxopN",
                   "Queue that manages packets belonging to AC_BE access class",
                   PointerValue (),
                   MakePointerAccessor (&REDWifiMac::GetBEQueue),
                   MakePointerChecker<EdcaTxopN> ())
    .AddAttribute ("BK_EdcaTxopN",
                   "Queue that manages packets belonging to AC_BK access class",
                   PointerValue (),
                   MakePointerAccessor (&REDWifiMac::GetBKQueue),
                   MakePointerChecker<EdcaTxopN> ())
    .AddTraceSource ("TxOkHeader",
                     "The header of successfully transmitted packet",
                     MakeTraceSourceAccessor (&REDWifiMac::m_txOkCallback),
                     "ns3::WifiMacHeader::TracedCallback")
    .AddTraceSource ("TxErrHeader",
                     "The header of unsuccessfully transmitted packet",
                     MakeTraceSourceAccessor (&REDWifiMac::m_txErrCallback),
                     "ns3::WifiMacHeader::TracedCallback")
    .AddConstructor<REDWifiMac> ()
  ;
  return tid;
}

void
REDWifiMac::FinishConfigureStandard (enum WifiPhyStandard standard)
{
  uint32_t cwmin;
  uint32_t cwmax;

  switch (standard)
    {
    case WIFI_PHY_STANDARD_holland:
    case WIFI_PHY_STANDARD_80211a:
    case WIFI_PHY_STANDARD_80211g:
    case WIFI_PHY_STANDARD_80211_10MHZ:
    case WIFI_PHY_STANDARD_80211_5MHZ:
    case WIFI_PHY_STANDARD_80211n_5GHZ:
    case WIFI_PHY_STANDARD_80211n_2_4GHZ:
    case WIFI_PHY_STANDARD_80211ac:
      cwmin = 15;
      cwmax = 1023;
      break;
    case WIFI_PHY_STANDARD_80211b:
      cwmin = 31;
      cwmax = 1023;
      break;
    default:
      NS_FATAL_ERROR ("Unsupported WifiPhyStandard in REDWifiMac::FinishConfigureStandard ()");
    }

  //The special value of AC_BE_NQOS which exists in the Access
  //Category enumeration allows us to configure plain old DCF.
  ConfigureDcf (m_dca, cwmin, cwmax, AC_BE_NQOS);

  //Now we configure the EDCA functions
  for (EdcaQueues::iterator i = m_edca.begin (); i != m_edca.end (); ++i)
    {
      ConfigureDcf (i->second, cwmin, cwmax, i->first);
    }
}

void
REDWifiMac::TxOk (const WifiMacHeader &hdr)
{
  NS_LOG_FUNCTION (this << hdr);
  m_txOkCallback (hdr);
}

void
REDWifiMac::TxFailed (const WifiMacHeader &hdr)
{
  NS_LOG_FUNCTION (this << hdr);
  m_txErrCallback (hdr);
}

} //namespace ns3
