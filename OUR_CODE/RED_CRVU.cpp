
#include "RED_CRVU.h"

using namespace std;

/*int main() {
	
	 cout << randomNumber() << endl;
	
	return 0;
}*/

	double RED_CRVU::randomNumber(){
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0, 1);
		return dis(gen);
	}