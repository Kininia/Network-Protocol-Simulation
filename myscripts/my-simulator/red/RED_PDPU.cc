
#include "RED_PDPU.h"
#include "ns3/log.h"

namespace ns3{

NS_LOG_COMPONENT_DEFINE ("RED_PDPU");

//NS_OBJECT_ENSURE_REGISTERED (RED_MainBuff);

RED_PDPU::RED_PDPU() : 
start_pdrop(false),
enqueue_now(false),
done_pdrop(false),
drop_early(false),
pb(0),
pa(0),
count(0),
qw(0),
minth(0),
maxth(0),
avgQ(0),
maxp(0),
tm(0)
{
	qw = 0.002;
	minth = 10;
	maxth = 30;

	maxp = 0.025;
}

void RED_PDPU::calc_avgQ(int ql){
	avgQ = qw*ql + (1 - qw)*avgQ;
}


void RED_PDPU::check_avgQ(){
	NS_LOG_FUNCTION(this);
	enqueue_now = false;
	done_pdrop = false;
	drop_early = false;
			
	if (avgQ < minth){
		count = 0;
		enqueue_now = true;
	} 
	else if(minth <= avgQ && avgQ < maxth){
		count = count + 1;
		calc_pb();
		done_pdrop = true;
	}
	else if (avgQ >= maxth){
		count = 0;		
		calc_pa();		
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
