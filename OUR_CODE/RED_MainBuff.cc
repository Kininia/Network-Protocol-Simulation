/*
 * Most of the methods are a direct copy of REDWifiMacQueue of the Ns3 class files.
 * The main reason for a copy and not inheritance is because we need to swap the use
 * of the REDWifiMacQueue file with the RED_MainBuff file for RED to work properly over WiFi.
 *
 *
 * The RED_MainBuff class adds RED functionality to WiFi in Ns3.
 * Most of the functionality lies in the classes RED_CRVU, RED_PDPU and RED_RPDU.
 * The big difference is in the methods Enqueue and RealEnqueue. Enqueue has logic for the RED algorithm.
 * The RED logic is run in Enqueue and then if a packet is to be enqueued according to the algorithm RealEnqueue is run.
 */

#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "RED_MainBuff.h"
#include "../../../ns-3.24/src/wifi/model/qos-blocked-destinations.h"
	
	
using namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (RED_MainBuff);

RED_MainBuff::Item::Item (Ptr<const Packet> packet, const WifiMacHeader &hdr, Time tstamp)
  : packet (packet),
    hdr (hdr),
    tstamp (tstamp)
{
}

TypeId RED_MainBuff::GetTypeId (void){
  static TypeId tid = TypeId ("ns3::RED_MainBuff")
    .SetParent<Object> ()
    .SetGroupName ("Wifi")
    .AddConstructor<RED_MainBuff> ()
    .AddAttribute ("MaxPacketNumber", "If a packet arrives when there are already this number of packets, it is dropped.",
                   UintegerValue (400),
                   MakeUintegerAccessor (&RED_MainBuff::m_maxSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("MaxDelay", "If a packet stays longer than this delay in the queue, it is dropped.",
                   TimeValue (MilliSeconds (500.0)),
                   MakeTimeAccessor (&RED_MainBuff::m_maxDelay),
                   MakeTimeChecker ())
  ;
  return tid;
}

RED_MainBuff::RED_MainBuff()
  : m_size (0)
{
}

RED_MainBuff::~RED_MainBuff()
{
  Flush ();
}

void RED_MainBuff::SetMaxSize(uint32_t maxSize){
	m_maxSize = maxSize;
}

void RED_MainBuff::SetMaxSize(Time delay){
	m_maxDelay = delay;
}

uint32_t RED_MainBuff::GetMaxSize(){
	return m_maxSize;
}

Time RED_MainBuff::GetMaxDelay(){
	return m_maxDelay;
}

bool RED_MainBuff::Enqueue(Ptr<const Packet> packet, const WifiMacHeader &hdr){
	
	try{
		
		PDPU.check_avgQ();
		enqueue_now = PDPU.getState_enqueueNow();
		drop_early = PDPU.getState_dropEarly();
		done_pdrop = PDPU.getState_donePdrop();
		
		if (enqueue_now){
			RED_MainBuff::RealEnqueue(packet, &hdr);
			PDPU.setState_enqueueNow(false);
		}
		else if (done_pdrop){
			RPDU.calculate();
			done_drop = RPDU.getState_doneDrop();
			discard_now = RPDU.getState_discardNow();
			if (done_drop){
				RED_MainBuff::RealEnqueue(packet, &hdr);
				RPDU.setState_doneDrop(false);
			}
			else if (discard_now){
				return;
				//Throw away the packet.
			}
			else {
				throw 1;
			}
			
		}
		else if (drop_early){
			PDPU.calc_pb();
			PDPU.calc_pa();
			//TODO
		}
		else {
			throw 2;
		}
	}
	
	catch (uint32_t param){
		if (param == 1){
			std::cout <<"If-done_pdrop, default, Exception in RED_MainBuff::enqueue"<endl;
		}
		else if (param == 2){
			std::cout <<"If-never entered parameter, default, Exception in RED_MainBuff::enqueue"<endl;
		}
	}
}

void RED_MainBuff::RealEnqueue(Ptr<const Packet> packet, const WifiMacHeader &hdr){
	try{
		Time now = Simulator::Now();
		m_queue.push_back(Item (packet, hdr, now))
		m_size++;
	}
	catch (...){
		std::cout << "Error in RED_MainBuff::realEnqueue" << endl; 
	}
}

void RED_MainBuff::Cleanup(){
	if (m_queue.empty()){
		return;
	}
	
	Time now = Simulator::Now();
	uint32_t n = 0;
	for (PacketQueue i = m_queue.begin(); i != m_queue.end();){
		if (i->tstamp + m_maxDelay > now){
			i++;
		}
		else{
			i = m_queue.erase(i);
			n++;
		}
	}
	m_size -= n;
}

packet RED_MainBuff::Dequeue(WifiMacHeader *hdr){
	Cleanup();
	if (!m_queue.empty()){
		Item i = m_queue.front();
		m_queue.pop_front();
		m_size --;
		*hdr = i.hdr;
		return i.packet;
	}
	//OLD CODE
	/*packet p = m_queue.front();
	m_queue.pop();
	return p;*/
}

Ptr<const Packet> RED_MainBuff::Peek (WifiMacHeader *hdr){
	  Cleanup();
	  if (!m_queue.empty()){
		  Item i = m_queue.front();
		  *hdr = i.hdr;
		  return i.packet;
	  }
	  return 0;
}

/*Searches through the queue for a packet identified by Tid and adress. If no packet is found
the initialized packet with value 0 is returned.*/
Ptr<const Packet> RED_MainBuff::DequeueByTidAndAddress (WifiMacHeader *hdr, uint8_t tid, WifiMacHeader::AddressType type, Mac48Address addr){
	Cleanup();
	Ptr<const Packet> packet = 0;
	if (!m_queue.empty()){
		PacketQueueI it = 0;
		for (it = m_queue.begin(); it != m_queue.end(); ++it){
			if (it->hdr.IsQosData()){
				if (GetAddressForPacket (type, it) == dest && it->hdr.GetQosTid() == tid){
					packet = it->packet;
					*hdr = it->hdr;
					m_queue.erase(it);
					m_size--;
					break;
				}
			}
		}
	}
	return packet;
}

Ptr<const Packet> RED_MainBuff::PeekByTidAndAddress (WifiMacHeader *hdr, uint8_t tid, WifiMacHeader::AddressType type, Mac48Address addr, Time *timestamp){											 
	Cleanup();
	if (!m_queue.empty()){
		PacketQueueI it;
		for (it = m_queue.begin(); it !=m_queue.end();++it){
			if (it->hdr.IsQosData()){
				if (GetAddressForPacket(type,it) == dest && it->hdr.GetQosTid() == tid){
					*hdr = it->hdr;
					*timestamp = it->tstamp;
					return it->packet;
				}
			}
		}
	}
	return 0;
}


bool RED_MainBuff::IsEmpty(){
	Cleanup();
	return m_queue.empty();
}

uint32_t  RED_MainBuff::GetSize (){
	Cleanup();
	return m_size;
}

void RED_MainBuff::Flush (void){
	m_queue.erase(m_queue.begin(9,m_queue.end());
	m_size = 0;
}

Mac48Address RED_MainBuff::GetAddressForPacket (WifiMacHeader::AddressType type, PacketQueueI it){
	if (type == WifiMacHeader::ADDR1){
		return it->hdr.GetAddr1();
	}
	if (type == WifiMacHeader::ADDR2){
		return it->hdr.GetAddr2();
	}
	if (type == WifiMacHeader::ADDR3){
		return it->hdr.GetAddr3();
	}
	return 0;
}

bool RED_MainBuff::Remove (Ptr<const Packet> packet){
	PacketQueueI it = m_queue.begin();
	for (; it !=m_queue.end(); it++){
		if (it->packet == packet){
			m_queue.erase(it);
			m_size--;
			return true;
		}
	}
	return false;
}

void RED_MainBuff::PushFront (Ptr<const Packet> packet, const WifiMacHeader &hdr){
		Cleanup();
		if (m_size == m_maxSize){
			return;
		}
		Time now = Simulator::Now();
		m_queue.push_front(Item (packet, hdr, now));
		m_size++;
}

uint32_t RED_MainBuff::GetNPacketsByTidAndAddress (uint8_t tid, WifiMacHeader::AddressType type, Mac48Address addr){
	Cleanup();
	uint32_t nPackets = 0;
	if (!m_queue.empty()){
		PacketQueueI it;
		for (it = m_queue.begin(9; it != m_queue.end(); it++){
			if (GetAddressForPacket(type, it) == addr){
				if (it->hdr.IsQosData(9 && it->hdr.GetQosTid() == tid){
					nPackets++;
				}
			}
		}
	}
	return nPackets;
}

Ptr<const Packet> RED_MainBuff::DequeueFirstAvailable (WifiMacHeader *hdr, Time &tStamp, const QosBlockedDestinations *blockedPackets){
	Cleanup();
	Ptr<const Packet> packet = 0;
	for (PacketQueueI it = m_queue.begin(); it !=m_queue.end(); it++){
		if (!it->hdr.IsQosData() || !blockedPackets->IsBlocked(it->hdr.GetAddr1(), it->hdr.GetQosTid())){
			*hdr = it->hdr;
			timestamp = it->tstamp;
			packet = it->packet;
			m_queue.erase(it);
			m_size--;
			return packet;
		}
	}
	return packet;
}

Ptr<const Packet> RED_MainBuff::PeekFirstAvailable (WifiMacHeader *hdr, Time &tStamp, const QosBlockedDestinations *blockedPackets){
	Cleanup();
	for (PacketQueueI it = m_queue.begin(); it != m_queue.end(); it++){
		if (!it->hdr.IsQosData() || !blockedPackets->IsBlocked(it->hdr.GetAddr1(), it->hdr.GetQosTid())){
			*hdr = it->hdr;
			timestamp = it-tstamp;
			return it->packet;
		}
	}
	return 0;
}

} //namespace ns3 ends.