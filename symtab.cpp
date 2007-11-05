#include "symtab.hpp"

Symtab::Symtab(const Symtab* parent) : parent_(parent)
{
}


bool Symtab::eqstr::operator()(const char* s1, const char* s2) const
{
	return strcmp(s1, s2) == 0;
}



Symtab::Symtab() : parent_(NULL) { };



bool Symtab::add_var(const char* name, const Variable& v)
{
	ht_t::const_iterator it = ht_.find(name);
	if (it != ht_.end())
	{
		return false;
	}
	ht_[name] = v;
	return true;
}



Variable Symtab::get_var(const char *name) const
{
	ht_t::const_iterator it = ht_.find(name);

	if (it == ht_.end())
	{
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
