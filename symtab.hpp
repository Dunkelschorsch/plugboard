#ifndef _SYMTAB_HPP
#define _SYMTAB_HPP

#include "exceptions.hpp"
#include "variable.hpp"
#include <map>

/** The Symtab class
 */
class Symtab
{
public:
	Symtab();

	Symtab(const Symtab* parent);

	bool add_var(const std::string& name, const Variable& v);

	const Variable& get_var(const std::string& name) const throw(UndefinedVariableException);

private:
	typedef std::map< const std::string, Variable > ht_t;

	ht_t ht_;

	const Symtab* parent_;
};

#endif
