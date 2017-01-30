// CRVU.cpp : Defines the entry point for the console application.
//


#include <random>
#include <iostream>
#include <stdint.h>
#include <bitset>

using namespace std;
public:
	double randomNumber ();

int main() {
	
	 cout << randomNumber() << endl;
	
	return 0;
}

double RED_CRVU::randomNumber(){
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);
	return dis(gen);
}