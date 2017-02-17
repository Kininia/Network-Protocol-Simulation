
#include "RED_PDPU.h"

namespace ns3{

void RED_PDPU::calc_avgQ(){
	get_ql();
	avgQ = qw*ql + (1 - qw)*avgQ;
}


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
						
						
double RED_PDPU::calc_pb(){
	pb = maxp * ((avgQ - minth)/(maxth - minth));
	return pb;
}

double RED_PDPU::calc_pa(){
	
	pa =pb/(1 - (count * pb));
	return pa;
	
}
		

double RED_PDPU::get_pb(){
			
	return pb;
}


void RED_PDPU::get_ql(){
	RED_MainBuff buff;
	ql = buff.GetSize();
}	


bool RED_PDPU::getState_enqueueNow(){
	return enqueue_now;
}


bool RED_PDPU::getState_donePdrop(){
	return done_pdrop;
}


bool RED_PDPU::getState_dropEarly(){
	return drop_early;
}


void RED_PDPU::setState_enqueueNow(bool b){
	enqueue_now = b;
}


void RED_PDPU::setState_donePdrop(bool b){
	done_pdrop = b;
}


void RED_PDPU::setState_dropEarly(bool b){
	drop_early = b;
}
}