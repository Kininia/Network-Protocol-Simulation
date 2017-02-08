
#include "RED_PDPU.h"

using namespace std;


/**
* Calculate average length of the queue.
* 
*/
void RED_PDPU::calc_avgQ(){
			
	avgQ = qw*ql + (1 - qw)*avgQ;
}


/**
 * Checks average queue length and
 * sets respective variable depending on length.
 *  
 */
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
						
						
/**
* Calculates drop probability for a packet.
* 
* \return drop probability of the pakage.
*/
double RED_PDPU::calc_pb(){
	pb = maxp * ((avgQ - minth)/(maxth - minth));
	return pb;
}

/**
* Calculates a variable used for calculating drop probability.
* 
* \return pa, that is used in drop probability
*/
double RED_PDPU::calc_pa(){
	
	pa =pb/(1 - (count * pb));
	return pa;
	
}
		

/**
 * Getter for drop probability.
 * 
 * \return drop probability of pakage.
 */
double RED_PDPU::get_pb(){
			
	return pb;
}


/**
* Getter for queue length from MainBuff
*
*/
void RED_PDPU:: get_ql(){
	ql = RED_MainBuff::getSize();
}	


/**
* Getter for enqueue_now state.
*
* \return true if enqueue_now is true, otherwise false.
*/
bool RED_PDPU:: getState_enqueueNow(){
	return enqueue_now;
}

/**
* Getter for done_pdrop state.
* 
* \return true if done_pdrop is true, otherwise false.
*/
bool RED_PDPU:: getState_donePdrop(){
	return done_pdrop;
}

/**
* Getter for drop_early state.
* 
* \return true if drop_early is true, otherwise false.
*/
bool RED_PDPU:: getState_dropEarly(){
	return drop_early;
}

/**
* Setter for enqueue_now state.
* 
* \param what to set state as
*/
bool RED_PDPU:: setState_enqueueNow(bool b){
	this.enqueue_now = b;
}


/**
* Setter for enqueue_now state.
* 
* \param what to set state as
*/
bool RED_PDPU:: setState_donePdrop(bool b){
	this.done_pdrop = b;
}


/**
* Setter for drop_early state.
* 
* \param what to set state as* 
*/
bool RED_PDPU:: setState_dropEarly(bool b){
	this.drop_early = b;
}