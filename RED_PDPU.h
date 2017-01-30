

#ifndef WIFI_MAC_QUEUE_H
#define WIFI_MAC_QUEUE_H
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
		bool return_state();
		
	private:
		double pb;
		int count;
		
};







#endif