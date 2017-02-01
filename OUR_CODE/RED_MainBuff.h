#ifndef RED_PDPU_H
#define RED_PDPU_H


#include <stdint>
#include "RED_MainBuff.h"
#include "ns3/packet.h"
#include <queue.h>
#include "RED_PDPU.h"
#include "RED_RPDU.h"
#include "RED_CRVU.h"

using namespace std;

class RED_MainBuff {
	
	public:
		bool cell_ready = false;
		bool start_pdrop = false;
};

#endif