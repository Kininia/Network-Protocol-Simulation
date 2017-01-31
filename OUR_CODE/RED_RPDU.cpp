
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
		}
		else {
			discard_now = false;
		}
	}
	bool RED_RPDU::getState_doneDrop(){
		return done_drop
		
	}

	bool RED_RPDU::getState_discardNow(){
		return discard_now;
	}
	

//For testing class in conjunction with RED_CRVU
/*int main() {

	RED_RPDU RPDU;
	RPDU.calculate();
	return 0;
}*/