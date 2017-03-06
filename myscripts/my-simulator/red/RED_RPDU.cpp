

#include "RED_RPDU.h"

#include "RED_CRVU.h"

namespace ns3{

RED_RPDU::RED_RPDU() :
discard_now(false),
done_drop(false)
{}

void RED_RPDU::calculate(double pdrop) {

	double q = RED_CRVU::randomNumber();
	
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
