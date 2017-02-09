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
		bool RED_RPDU::getState_doneDrop();
		
		/**
		* Getter for discard_now state.
		* 
		* \return true if discard_now is true, otherwise false.
		*/
		bool RED_RPDU::getState_discardNow();
		
		/**
		* Setter for done_drop state.
		* 
		* \param what to set state as.
		*/
		bool RED_RPDU::setState_doneDrop();
		
		/**
		* Setter for discard_now state.
		* 
		* \param what to set state as. 
		*/
		bool RED_RPDU::setState_discardNow();
};
#endif