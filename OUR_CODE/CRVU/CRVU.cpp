// CRVU.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <stdint.h>
#include <bitset>

using namespace std;

int main() {
	bitset<8> seed(string("00000010"));
	cout << seed << endl;
	unsigned int feedback = seed[0] ^ seed[1] ^ seed[2] ^ seed[3] ^ seed[4] ^ seed[5] ^ seed[6] ^ seed[7]; // Creates feedback bit from seed.
	cout << feedback << endl;
	return 0;
}

