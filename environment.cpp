#include "environment.hpp"
#include <tr1/unordered_map>


typedef pimpl< Environment >::implementation EnvironmentImpl;


Environment::Environment() : base() { }


template<>
struct pimpl< Environment >::implementation
{
	implementation();

	std::tr1::unordered_map< std::string, boost::any > properties;
};


EnvironmentImpl::implementation() :
	properties()
{
	properties["threading"] = false;
}


const boost::any& Environment::get(const std::string& prop)
{
	return (*this)->properties[prop];
}


Environment& Environment::set(const std::string& prop, boost::any val)
{
	(*this)->properties[prop] = val;
	return *this;
}
