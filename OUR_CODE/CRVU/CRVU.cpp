// CRVU.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <stdint.h>
#include <bitset>

using namespace std;
int main() {
	bitset<8> seed(string("00000010"));
	unsigned int feedback = seed[0] ^ seed[1] ^ seed[2] ^ seed[3] ^ seed[4] ^ seed[5] ^ seed[6] ^ seed[7];
	cout << "hello world" << endl;
	return 0;
}

