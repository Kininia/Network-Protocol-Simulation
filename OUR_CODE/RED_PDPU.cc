
#include "RED_PDPU.h"

using namespace std;


// Calculate average queue length.
void RED_PDPU::calc_avgQ(){
			
	avgQ = qw*ql + (1 - qw)*avgQ;
}


// Check average queue length, and tell witch drop state to use.
void RED_PDPU::check_avgQ(){
			
	if (avgQ < minth){
		count = 0; //try -1
		enqueue_now = true;
	} 
	else if(minth <= avgQ < maxth){
		count = count + 1;
		calc_pb();
		done_pdrop = true;
	}
	else if (avgQ >= maxth){
		count = 0;		
		calc_pb();		
		drop_early=true;
	}

}
						
						
// Calculate drop probability.	
void RED_PDPU::calc_pb(){
			
	double pa;
	pa = maxp * ((avgQ - minth)/(maxth - minth));
	pb =pa/(1 - (count * pa));
}
		

// Get drop probability.		
double RED_PDPU::get_pb(){
			
	return pb;
}

void RED_PDPU:: get_ql(){
	ql = RED_MainBuff::return_ql();
}	

bool RED_PDPU:: return_state(){
	if (enqueue_now = true){
		return enqueue_now;
	}
	else if (done_pdrop = true){
			return done_pdrop;
	}
	else if (drop_early = true){
		return drop_early;
	}
}

