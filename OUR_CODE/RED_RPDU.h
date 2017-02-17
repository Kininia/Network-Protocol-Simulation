#ifndef RED_RDPU_H
#define RED_RDPU_H

#include <stdint.h>
//#include <iostream>

#include "RED_PDPU.h"
#include "RED_CRVU.h"

namespace ns3{

class RED_RPDU{
	private:
		bool discard_now;
		bool done_drop;
		double pdrop;
		double q;

		
	public:
		
		/**
		* Compares two numbers from
		* CRVU & PDPU respectively and
		* sets different states true/false depending
		* on which is greater of them.
		* 
		*/
		void calculate();	
		
		/**
		* Getter for done_drop state.
		* 
		* \return true if done_drop is true, otherwise false.
		*/
		bool getState_doneDrop();
		
		/**
		* Getter for discard_now state.
		* 
		* \return true if discard_now is true, otherwise false.
		*/
		bool getState_discardNow();
		
		/**
		* Setter for done_drop state.
		* 
		* \param what to set state as.
		*/
		void setState_doneDrop(bool state);
		
		/**
		* Setter for discard_now state.
		* 
		* \param what to set state as. 
		*/
		void setState_discardNow(bool state);
};
}
#endif
