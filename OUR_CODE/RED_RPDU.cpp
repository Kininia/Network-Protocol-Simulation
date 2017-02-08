
#include "RED_RPDU.h"

using namespace std;
/**
* Compares two numbers from
* CRVU & PDPU respectively and
* sets different states true/false depending
* on which is greater of them.
* 
*/
void RED_RPDU::calculate() {
	RED_CRVU CRVU;
	RED_PDPU PDPU;
	
	pdrop = PDPU.get_PB();
	
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


/**
* Getter for done_drop state.
* 
* \return true if done_drop is true, otherwise false.
*/
bool RED_RPDU::getState_doneDrop(){
	return done_drop;
}


/**
* Getter for discard_now state.
* 
* \return true if discard_now is true, otherwise false.
*/
bool RED_RPDU::getState_discardNow(){
	return discard_now;
}


/**
* Setter for done_drop state.
* 
* \param what to set state as.
*/
bool RED_RPDU::setState_doneDrop(bool b){
	this.done_drop = b;
}


/**
* Setter for discard_now state.
* 
* \param what to set state as. 
*/
bool RED_RPDU::setState_discardNow(bool b){
	this.discard_now = b;
}