
#include "RED_RPDU.h"

using namespace std;

	void RED_RPDU::calculate() {
		RED_CRVU CRVU;
		RED_PDPU PDPU;
		pdrop = PDPU.get_PB();
		q = CRVU.randomNumber();
		cout << q << endl;
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
	
	bool RED_RPDU::setState_doneDrop(bool b){
		this.done_drop = b;
	}

	bool RED_RPDU::setState_discardNow(bool b){
		this.discard_now = b;
	}
	

//For testing class in conjunction with RED_CRVU
/*int main() {

	RED_RPDU RPDU;
	RPDU.calculate();
	return 0;
}*/