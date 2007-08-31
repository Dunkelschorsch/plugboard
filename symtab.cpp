#include "symtab.hpp"


bool Symtab::eqstr::operator()(const char* s1, const char* s2) const
{
	return strcmp(s1, s2) == 0;
}

Symtab::Symtab() { };

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

Variable Symtab::get_var(const char *name)
{
	ht_t::const_iterator it = ht_.find(name);
	if (it == ht_.end())
	{
		return Variable();
	}
	return ht_[name];
}
