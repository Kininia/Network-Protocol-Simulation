
#include "RED_CRVU.h"

using namespace std;

	double RED_CRVU::randomNumber(){
		/*random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0, 1);
		return dis(gen);*/

		// our initial starting seed is 5323
		static uint32_t seed = 5323.25;

		// Take the current seed and generate a new value from it
		// Due to our use of large constants and overflow, it would be
		// very hard for someone to predict what the next number is
		// going to be from the previous one.
		seed = (8253729 * seed + 2396403);
	 
		// Take the seed and return a value between 0 and 32767
		return seed  % 2;
	}
