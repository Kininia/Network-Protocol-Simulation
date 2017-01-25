// CRVU.cpp : Defines the entry point for the console application.
//


#include <random>
#include <iostream>
#include <stdint.h>
#include <bitset>

using namespace std;

double randomNumber ();

int main() {
	
	 cout << randomNumber() << endl;
	
	return 0;
}

double randomNumber(){
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);
	for (int i= 0; i<20; i++){

		//cout << dis(gen) << endl;
	}
	return dis(gen);
}