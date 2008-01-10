#include "symtab.hpp"
#include <cassert>
#ifndef NDEBUG
#include <iostream>
#endif

Symtab::Symtab(const Symtab* parent) : parent_(parent)
{
}

/*
bool Symtab::eqstr::operator()(const char* s1, const char* s2) const
{
	return strcmp(s1, s2) == 0;
}*/



Symtab::Symtab() : parent_(NULL) { }



bool Symtab::add_var(const std::string& name, const Variable& v)
{
	assert(static_cast< bool >(v) == true);

	ht_t::const_iterator it = ht_.find(name);
	if (it != ht_.end())
	{
		return false;
	}
	ht_[name] = v;
	return true;
}



Variable Symtab::get_var(const std::string& name) const
{
#ifndef NDEBUG
	std::cout << "Symtab::get_var(). currently holding " << ht_.size() << " variables." << std::endl;
#endif
	ht_t::const_iterator it = ht_.find(name);

	if (it == ht_.end())
	{
#ifndef NDEBUG
	std::cout << "cannot find variable '" << name << "'" << std::endl;
	std::cout << "available variables are:" << std::endl;
	for(ht_t::const_iterator vars=ht_.begin(); vars != ht_.end(); vars++)
	{
		std::cout << vars->first << std::endl;
	}
#endif
		if(parent_ != NULL)
		{
			return parent_->get_var(name);
		}
		else
		{
			return Variable();
		}
	}
	return it->second;
}
