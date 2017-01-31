#ifndef RED_PDPU_H
#define RED_PDPU_H

#include <stdint.h>
#include <iostream>

#include "RED_CRVU.h"

class RED_RPDU{
	private:
		bool discard_now = false;
		bool done_drop = false;
		double pdrop;
		double q;
	public:
			void calculate();	
			bool RED_RPDU::getState_doneDrop();
			bool RED_RPDU::getState_discardNow();
};
#endif