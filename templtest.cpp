#include <iostream>
#include <vector>
#include <algorithm>
#include "signal.hpp"
#include "system.hpp"
#include "types.hpp"

struct SysDerivedImpl;
class SysDerived : public System
{
public:
	SysDerived();
	void hump();
protected:
	SysDerived(SysDerivedImpl &dd);
private:
	friend class SysDerivedImpl;

	inline SysDerivedImpl *d_func()
	{ return reinterpret_cast<SysDerivedImpl *>(d_ptr); }
	inline const SysDerivedImpl *d_func() const
	{ return reinterpret_cast<SysDerivedImpl *>(d_ptr); }
};


class SysDerivedImpl : public SystemImpl
{
public:
	double hump;
};

SysDerived::SysDerived() : System(*new SysDerivedImpl)
{

}

SysDerived::SysDerived(SysDerivedImpl &dd) : System(dd)
{

}


void SysDerived::hump()
{
	H_D(SysDerived);

	d->hump = 1.0;
	std::cout << " hump " << d->hump << std::endl;
}

int main(int argc, char **argv)
{
	SysDerived a;

	a.hump();

	int size=10;
	Signal *s;
	s = new RealSignal(size);

	real_vec_t v(size);
	std::fill(v.begin(), v.end(), 42.1);
	dynamic_cast< RealSignal* >(s)->put(&v);
	
	real_t* hump = 0;
	
	std::cout << dynamic_cast< RealSignal* >(s)->get_data()[0] << std::endl;
		
	delete s;
	return 0;
}
