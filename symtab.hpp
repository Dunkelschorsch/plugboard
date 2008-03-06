#ifndef SYMTAB_HPP
#define SYMTAB_HPP

#include <map>
class Variable;


/** The Symtab class
 */
class Symtab
{
public:
	Symtab();

	Symtab(const Symtab* parent);

	bool add_var(const std::string& name, const Variable& v);

	const Variable& get_var(const std::string& name) const;

private:
	typedef std::map< const std::string, Variable > ht_t;

	ht_t ht_;

	const Symtab* parent_;
};

#endif // SYMTAB_HPP
