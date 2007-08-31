#include <iostream>
#include <vector>
#include <algorithm>
#include "signal.hpp"
#include "types.hpp"


int main(int argc, char **argv)
{
	int size=10;
	Signal *s;
	s = new RealSignal(size);

	real_vec_t v(size);
	std::fill(v.begin(), v.end(), 42.1);
	dynamic_cast< RealSignal* >(s)->put(&v);
	
	real_t* hump = 0;
	
	//hump[0] = 1;
	std::cout << dynamic_cast< RealSignal* >(s)->get_data()[0];
		
	delete s;
	return 0;
}
