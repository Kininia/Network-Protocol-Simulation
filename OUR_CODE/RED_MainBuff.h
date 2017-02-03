
#ifndef RED_PDPU_H
#define RED_PDPU_H


#include <stdint>
#include "RED_MainBuff.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/packet.h"
#include <queue.h>
#include "RED_PDPU.h"
#include "RED_RPDU.h"
#include "RED_CRVU.h"

using namespace std;

class RED_MainBuff {
	
	public:
		bool cell_ready = false;
		bool start_pdrop = false;
		bool enqueue_now = false;
		bool done_drop = false;
		bool done_pdrop = false;
		bool drop_early = false;
		bool discard_now = false;
		
		void RED_MainBuff::setMaxSize(int s);

		void RED_MainBuff::getMaxSize();

		bool RED_MainBuff::enqueue(Ptr<const Packet> packet, const WifiMacHeader &hdr);

		packet RED_MainBuff::dequeue(WifiMacHeader *hdr);

		bool RED_MainBuff::IsEmpty();

		uint32_t  RED_MainBuff::GetSize ();

		void PushFront (Ptr<const Packet> packet, const WifiMacHeader &hdr);

		Ptr<const Packet> Peek (WifiMacHeader *hdr);


		Ptr<const Packet> DequeueByTidAndAddress (WifiMacHeader *hdr, uint8_t tid, WifiMacHeader::AddressType type, Mac48Address addr);

		Ptr<const Packet> PeekByTidAndAddress (WifiMacHeader *hdr, uint8_t tid, WifiMacHeader::AddressType type, Mac48Address addr, Time *timestamp);

		bool Remove (Ptr<const Packet> packet);

		uint32_t GetNPacketsByTidAndAddress (uint8_t tid, WifiMacHeader::AddressType type, Mac48Address addr);

		Ptr<const Packet> DequeueFirstAvailable (WifiMacHeader *hdr, Time &tStamp, const QosBlockedDestinations *blockedPackets);

		Ptr<const Packet> PeekFirstAvailable (WifiMacHeader *hdr, Time &tStamp, const QosBlockedDestinations *blockedPackets);
		void Flush (void);
};

#endif