#include <iostream>
#include <stdint.h>
#include <bitset>

using namespace std;

/*class CRVU {
	public:
		//uint8_t seed = 00000010;
		unsigned char feedback;
	
	
	
};*/

int main () {
	bitset<8> seed (string("00000010"));
	unsigned int feedback = seed[0] ^ seed[1]^ seed[2]^ seed[3]^ seed[4]^ seed[5]^ seed[6]^ seed[7];
	cout << feedback <<endl;
	return 0;
}

