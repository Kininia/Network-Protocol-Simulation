#ifndef RED_PDPU_H
#define RED_PDPU_H

class RED_RPDU{
	private:
		bool discard_now;
		double pdrop;
		double q;
	public:
			void calculate();	
};
#endif