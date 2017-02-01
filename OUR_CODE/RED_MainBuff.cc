
#include "REDWifiMacQueue.h" // might not need to be included.

using namespace std;

int main(){
	
	RED_CRVU CRVU;
	RED_PDPU PDPU;
	RED_RPDU RPDU;
	queue<packet> fifoBuffer;
	
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

bool RED_MainBuff::enqueue(packet p){
	if (fifoBuffer.push(p)){
		return true;
	}
	else {
		return false;
	}
}

packet RED_MainBuff::dequeue(){
	packet p = fifoBuffer.front();
	fifoBuffer.pop();
	return p;
}
bool RED_MainBuff::isEmpty(){
	return fifoBuffer.empty();
}

int RED_MainBuff::getql(){
	return fifoBuffer.size();
}