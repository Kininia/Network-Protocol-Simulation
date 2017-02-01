
#include "REDWifiMacQueue.h" // might not need to be included.

using namespace std;

int main(){
	
	RED_CRVU CRVU;
	RED_PDPU PDPU;
	RED_RPDU RPDU;
	queue<packet> fifoBuffer;
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