#ifndef RED_PDPU_H
#define RED_PDPU_H

#include "RED_MainBuff.h"
#include "ns3/packet.h"
#include <queue.h>

using namespace std;

class RED_MainBuff {
	
	public:
		bool cell_ready = false;
		bool start_pdrop = false;
};

#endif