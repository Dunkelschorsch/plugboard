#include "environment.hpp"

typedef pimpl< Environment >::implementation EnvironmentImpl;


Environment::Environment() : base() { }


template<>
struct pimpl< Environment >::implementation
{
	implementation();

	bool threading;
};


EnvironmentImpl::implementation() :
	threading(false)
{

}


void Environment::set_threading(bool b)
{
	(*this)->threading = b;
}


bool Environment::threading_enabled()
{
	return (*this)->threading;
}
