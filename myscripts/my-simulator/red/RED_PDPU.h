

#ifndef RED_PDPU_H
#define RED_PDPU_H


namespace ns3{

class RED_PDPU {
	
	public:

		RED_PDPU();

		
		/**
		* Calculate average length of the queue.
		* 
		*/
		void calc_avgQ(int);

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
		double calc_pb();
		
		/**
		* Calculates a variable used for calculating drop probability.
		* 
		* \return pa, that is used in drop probability
		*/
		double calc_pa();

		/**
		* Getter for drop probability.
		* 
		* \return drop probability of pakage.
		*/
		double get_pb();
		
		/**
		* Getter for enqueue_now state.
		*
		* \return true if enqueue_now is true, otherwise false.
		*/
		bool getState_enqueueNow();

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
		void setState_enqueueNow(bool state);
		
		/**
		* Setter for enqueue_now state.
		* 
		* \param what to set state as
		*/
		void setState_donePdrop(bool state);
		
		/**
		* Setter for drop_early state.
		* 
		* \param what to set state as* 
		*/
		void setState_dropEarly(bool state);
		
	private:
		double pb;
		double pa;
		int count;

		bool start_pdrop;
		bool enqueue_now;
		bool done_pdrop;
		bool drop_early;
		double qw;
		int minth;
		int maxth;
		double avgQ;
		double maxp;
		double tm;
		
};

}
#endif
