

#ifndef RED_PDPU_H
#define RED_PDPU_H
#include "RED_MainBuff.h"

using namespace std;

class RED_PDPU {
	
	public:
		bool start_pdrop = false;
		bool enqueue_now = false;
		bool done_pdrop = false;
		bool drop_early = false;
		double qw;
		int minth;
		int maxth;
		double avgQ;
		int maxp;
		double tm;
		int ql;
		
		/**
		* Calculate average length of the queue.
		* 
		*/
		void calc_avqQ();

		/**
		* Checks average queue length and
		* sets respective variable depending on length.
		*  
		*/		
		void check_avgQ();
		
		/**
		* Calculates drop probability for a packet.
		* 
		* \return drop probability of the pakage.
		*/
		void calc_pb();
		
		/**
		* Calculates a variable used for calculating drop probability.
		* 
		* \return pa, that is used in drop probability
		*/
		void calc_pa();

		/**
		* Getter for drop probability.
		* 
		* \return drop probability of pakage.
		*/
		double get_pb();
		
		/**
		* Getter for queue length from MainBuff
		*
		*/
		void get_ql();
		
		/**
		* Getter for enqueue_now state.
		*
		* \return true if enqueue_now is true, otherwise false.
		*/
		bool getState_EnqueueNow();

		/**
		* Getter for done_pdrop state.
		* 
		* \return true if done_pdrop is true, otherwise false.
		*/
		bool getState_donePdrop();
		
		/**
		* Getter for drop_early state.
		* 
		* \return true if drop_early is true, otherwise false.
		*/
		bool getState_dropEarly();
		
		/**
		* Setter for enqueue_now state.
		* 
		* \param what to set state as
		*/
		bool setState_EnqueueNow();
		
		/**
		* Setter for enqueue_now state.
		* 
		* \param what to set state as
		*/
		bool setState_donePdrop();
		
		/**
		* Setter for drop_early state.
		* 
		* \param what to set state as* 
		*/
		bool setState_dropEarly();
		
	private:
		double pb;
		double pa;
		int count;
		
};







#endif