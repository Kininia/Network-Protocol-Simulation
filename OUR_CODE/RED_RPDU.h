#ifndef RED_PDPU_H
#define RED_PDPU_H

#include <stdint.h>
#include <iostream>

#include "RED_CRVU.h"

class RED_RPDU{
	private:
		bool discard_now = false;
		double pdrop;
		double q;
	public:
			void calculate();	
};
#endif