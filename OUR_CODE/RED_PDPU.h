

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
	
		void calc_avqQ();
		void check_avgQ();
		void calc_pb();
		double get_pb();
		bool getState_EnqueueNow();
		bool getState_donePdrop();
		bool getState_dropEarly();
		bool setState_EnqueueNow();
		bool setState_donePdrop();
		bool setState_dropEarly();
		
	private:
		double pb;
		double pa;
		int count;
		
};







#endif