/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005, 2009 INRIA
 * Copyright (c) 2009 MIRKO BANCHI
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 *          Mirko Banchi <mk.banchi@gmail.com>
 */

#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "REDWifiMacQueue.h"
#include "../../../ns-3.24/src/wifi/model/qos-blocked-destinations.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (REDWifiMacQueue);

REDWifiMacQueue::Item::Item (Ptr<const Packet> packet,
                          const WifiMacHeader &hdr,
                          Time tstamp)
  : packet (packet),
    hdr (hdr),
    tstamp (tstamp)
{
}

TypeId
REDWifiMacQueue::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::REDWifiMacQueue")
    .SetParent<Object> ()
    .SetGroupName ("Wifi")
    .AddConstructor<REDWifiMacQueue> ()
    .AddAttribute ("MaxPacketNumber", "If a packet arrives when there are already this number of packets, it is dropped.",
                   UintegerValue (400),
                   MakeUintegerAccessor (&REDWifiMacQueue::m_maxSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("MaxDelay", "If a packet stays longer than this delay in the queue, it is dropped.",
                   TimeValue (MilliSeconds (500.0)),
                   MakeTimeAccessor (&REDWifiMacQueue::m_maxDelay),
                   MakeTimeChecker ())
  ;
  return tid;
}

REDWifiMacQueue::REDWifiMacQueue ()
  : m_size (0)
{
}

REDWifiMacQueue::~REDWifiMacQueue ()
{
  Flush ();
}

void
REDWifiMacQueue::SetMaxSize (uint32_t maxSize)
{
  m_maxSize = maxSize;
}

void
REDWifiMacQueue::SetMaxDelay (Time delay)
{
  m_maxDelay = delay;
}

uint32_t
REDWifiMacQueue::GetMaxSize (void) const
{
  return m_maxSize;
}

Time
REDWifiMacQueue::GetMaxDelay (void) const
{
  return m_maxDelay;
}

void
REDWifiMacQueue::Enqueue (Ptr<const Packet> packet, const WifiMacHeader &hdr)
{
  Cleanup ();
  if (m_size == m_maxSize)
    {
      return;
    }
  Time now = Simulator::Now ();
  m_queue.push_back (Item (packet, hdr, now));
  m_size++;
}

void
REDWifiMacQueue::Cleanup (void)
{
  if (m_queue.empty ())
    {
      return;
    }

  Time now = Simulator::Now ();
  uint32_t n = 0;
  for (PacketQueueI i = m_queue.begin (); i != m_queue.end (); )
    {
      if (i->tstamp + m_maxDelay > now)
        {
          i++;
        }
      else
        {
          i = m_queue.erase (i);
          n++;
        }
    }
  m_size -= n;
}

Ptr<const Packet>
REDWifiMacQueue::Dequeue (WifiMacHeader *hdr)
{
  Cleanup ();
  if (!m_queue.empty ())
    {
      Item i = m_queue.front ();
      m_queue.pop_front ();
      m_size--;
      *hdr = i.hdr;
      return i.packet;
    }
  return 0;
}

Ptr<const Packet>
REDWifiMacQueue::Peek (WifiMacHeader *hdr)
{
  Cleanup ();
  if (!m_queue.empty ())
    {
      Item i = m_queue.front ();
      *hdr = i.hdr;
      return i.packet;
    }
  return 0;
}

Ptr<const Packet>
REDWifiMacQueue::DequeueByTidAndAddress (WifiMacHeader *hdr, uint8_t tid,
                                      WifiMacHeader::AddressType type, Mac48Address dest)
{
  Cleanup ();
  Ptr<const Packet> packet = 0;
  if (!m_queue.empty ())
    {
      PacketQueueI it;
      for (it = m_queue.begin (); it != m_queue.end (); ++it)
        {
          if (it->hdr.IsQosData ())
            {
              if (GetAddressForPacket (type, it) == dest
                  && it->hdr.GetQosTid () == tid)
                {
                  packet = it->packet;
                  *hdr = it->hdr;
                  m_queue.erase (it);
                  m_size--;
                  break;
                }
            }
        }
    }
  return packet;
}

Ptr<const Packet>
REDWifiMacQueue::PeekByTidAndAddress (WifiMacHeader *hdr, uint8_t tid,
                                   WifiMacHeader::AddressType type, Mac48Address dest, Time *timestamp)
{
  Cleanup ();
  if (!m_queue.empty ())
    {
      PacketQueueI it;
      for (it = m_queue.begin (); it != m_queue.end (); ++it)
        {
          if (it->hdr.IsQosData ())
            {
              if (GetAddressForPacket (type, it) == dest
                  && it->hdr.GetQosTid () == tid)
                {
                  *hdr = it->hdr;
                  *timestamp = it->tstamp;
                  return it->packet;
                }
            }
        }
    }
  return 0;
}

bool
REDWifiMacQueue::IsEmpty (void)
{
  Cleanup ();
  return m_queue.empty ();
}

uint32_t
REDWifiMacQueue::GetSize (void)
{
  return m_size;
}

void
REDWifiMacQueue::Flush (void)
{
  m_queue.erase (m_queue.begin (), m_queue.end ());
  m_size = 0;
}

Mac48Address
REDWifiMacQueue::GetAddressForPacket (enum WifiMacHeader::AddressType type, PacketQueueI it)
{
  if (type == WifiMacHeader::ADDR1)
    {
      return it->hdr.GetAddr1 ();
    }
  if (type == WifiMacHeader::ADDR2)
    {
      return it->hdr.GetAddr2 ();
    }
  if (type == WifiMacHeader::ADDR3)
    {
      return it->hdr.GetAddr3 ();
    }
  return 0;
}

bool
REDWifiMacQueue::Remove (Ptr<const Packet> packet)
{
  PacketQueueI it = m_queue.begin ();
  for (; it != m_queue.end (); it++)
    {
      if (it->packet == packet)
        {
          m_queue.erase (it);
          m_size--;
          return true;
        }
    }
  return false;
}

void
REDWifiMacQueue::PushFront (Ptr<const Packet> packet, const WifiMacHeader &hdr)
{
  Cleanup ();
  if (m_size == m_maxSize)
    {
      return;
    }
  Time now = Simulator::Now ();
  m_queue.push_front (Item (packet, hdr, now));
  m_size++;
}

uint32_t
REDWifiMacQueue::GetNPacketsByTidAndAddress (uint8_t tid, WifiMacHeader::AddressType type,
                                          Mac48Address addr)
{
  Cleanup ();
  uint32_t nPackets = 0;
  if (!m_queue.empty ())
    {
      PacketQueueI it;
      for (it = m_queue.begin (); it != m_queue.end (); it++)
        {
          if (GetAddressForPacket (type, it) == addr)
            {
              if (it->hdr.IsQosData () && it->hdr.GetQosTid () == tid)
                {
                  nPackets++;
                }
            }
        }
    }
  return nPackets;
}

Ptr<const Packet>
REDWifiMacQueue::DequeueFirstAvailable (WifiMacHeader *hdr, Time &timestamp,
                                     const QosBlockedDestinations *blockedPackets)
{
  Cleanup ();
  Ptr<const Packet> packet = 0;
  for (PacketQueueI it = m_queue.begin (); it != m_queue.end (); it++)
    {
      if (!it->hdr.IsQosData ()
          || !blockedPackets->IsBlocked (it->hdr.GetAddr1 (), it->hdr.GetQosTid ()))
        {
          *hdr = it->hdr;
          timestamp = it->tstamp;
          packet = it->packet;
          m_queue.erase (it);
          m_size--;
          return packet;
        }
    }
  return packet;
}

Ptr<const Packet>
REDWifiMacQueue::PeekFirstAvailable (WifiMacHeader *hdr, Time &timestamp,
                                  const QosBlockedDestinations *blockedPackets)
{
  Cleanup ();
  for (PacketQueueI it = m_queue.begin (); it != m_queue.end (); it++)
    {
      if (!it->hdr.IsQosData ()
          || !blockedPackets->IsBlocked (it->hdr.GetAddr1 (), it->hdr.GetQosTid ()))
        {
          *hdr = it->hdr;
          timestamp = it->tstamp;
          return it->packet;
        }
    }
  return 0;
}

} //namespace ns3
