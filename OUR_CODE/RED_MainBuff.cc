
#include "REDWifiMacQueue.h" // might not need to be included.

using namespace std;

private:
	RED_CRVU CRVU;
	RED_PDPU PDPU;
	RED_RPDU RPDU;
	queue<packet> fifoBuffer;
	int queueMaxSize;
	

int main(){
	
	
	
	PDPU.check_avgQ();
	enqueue_now = PDPU.getState_enqueueNow();
	drop_early = PDPU.getState_dropEarly();
	done_pdrop = PDPU.getState_donePdrop();
	
	if (enqueue_now){
		fifoBuffer.enqueue(xxx);
		PDPU.setState_enqueueNow(false);
	}
	else if (done_pdrop){
		RPDU.calculate();
		done_drop = RPDU.getState_doneDrop();
		discard_now = RPDU.getState_discardNow();
		if (done_drop){
			fifoBuffer.enqueue(xxx);
			RPDU.setState_doneDrop(false);
		}
		else if (discard_now){
			//TODO
			//Throw away the packet.
		}
	}
	else if (drop_early){
		PDPU.calc_pb();
		PDPU.calc_pa();
		//TODO
		//Do when we're actually conscious.
	}
}

void RED_MainBuff::setMaxSize(int s){
	this.queueMaxSize = s;
}

void RED_MainBuff::getMaxSize(){
	return this.queueMaxSize;
}

bool RED_MainBuff::enqueue(Ptr<const Packet> packet, const WifiMacHeader &hdr){
	if (fifoBuffer.push(p)){
		return true;
	}
	else {
		return false;
	}
}

packet RED_MainBuff::dequeue(WifiMacHeader *hdr){
	packet p = fifoBuffer.front();
	fifoBuffer.pop();
	return p;
}
bool RED_MainBuff::IsEmpty(){
	return fifoBuffer.empty();
}

uint32_t  RED_MainBuff::GetSize (){
	return fifoBuffer.size();
}

void PushFront (Ptr<const Packet> packet, const WifiMacHeader &hdr){
	
}

Ptr<const Packet> Peek (WifiMacHeader *hdr){
	  
}


Ptr<const Packet> DequeueByTidAndAddress (WifiMacHeader *hdr,
                                            uint8_t tid,
                                            WifiMacHeader::AddressType type,
                                            Mac48Address addr){
}

Ptr<const Packet> PeekByTidAndAddress (WifiMacHeader *hdr,
                                         uint8_t tid,
                                         WifiMacHeader::AddressType type,
                                         Mac48Address addr,
                                         Time *timestamp){											 
}

bool Remove (Ptr<const Packet> packet){
	
}

uint32_t GetNPacketsByTidAndAddress (uint8_t tid,
                                       WifiMacHeader::AddressType type,
                                       Mac48Address addr){
										   
}

Ptr<const Packet> DequeueFirstAvailable (WifiMacHeader *hdr,
                                           Time &tStamp,
                                           const QosBlockedDestinations *blockedPackets){
											   
}

Ptr<const Packet> PeekFirstAvailable (WifiMacHeader *hdr,
                                        Time &tStamp,
                                        const QosBlockedDestinations *blockedPackets){
											
}

void Flush (void){
	
}