#ifndef _SYMTAB_HPP
#define _SYMTAB_HPP

#include "variable.hpp"
#include <ext/hash_map> // in namespace __gnu_cxx

using __gnu_cxx::hash_map;
using __gnu_cxx::hash;

/** The Symtab class
 */
class Symtab
{
public:
	Symtab();

	bool add_var(const char* name, const Variable& v);

	Variable get_var(const char* name);

private:

	struct eqstr
	{
		bool operator()(const char* s1, const char* s2) const;
	};

	typedef hash_map<const char*, Variable, hash<const char*>, eqstr> ht_t;

	ht_t ht_;
};

#endif
