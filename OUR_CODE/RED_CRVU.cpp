
#include "RED_CRVU.h"

using namespace std;
/**
* Generates a random number between 0-1.
*/
	double RED_CRVU::randomNumber(){
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0, 1);
		return dis(gen);
	}