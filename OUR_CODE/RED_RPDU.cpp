

#include "RED_RPDU.h"

namespace ns3{

void RED_RPDU::calculate() {
	RED_PDPU PDPU;
	RED_CRVU CRVU;
	
	
	pdrop = PDPU.get_pb();
	
	q = CRVU.randomNumber();
	
	if (q <= pdrop) {
		discard_now = true;
		done_drop = false;
	}
	
	else {
		discard_now = false;
		done_drop = true;
	}
}

bool RED_RPDU::getState_doneDrop(){
	return done_drop;
}

bool RED_RPDU::getState_discardNow(){
	return discard_now;
}

void RED_RPDU::setState_doneDrop(bool b){
	done_drop = b;
}


void RED_RPDU::setState_discardNow(bool b){
	discard_now = b;
}
}