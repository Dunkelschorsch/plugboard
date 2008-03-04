#ifndef PARAMETER_HPP
#define PARAMETER_HPP

#include "types/base.hpp"
#include "visibility.hpp"
#include <vector>

class Variable;
class ConstraintBase;


class DSOEXPORT Parameter
{
public:
	Parameter(void * data, type_t type, const std::string& description);
	Parameter(const Parameter&);
	Parameter& operator=(const Parameter&);
	~Parameter();

	type_t get_type() const;

	void * get_data() const;

	const std::string& get_description() const;

	Parameter* add_constraint(const ConstraintBase* c);

	std::vector< const ConstraintBase* >& get_constraints();

	bool is_convertible_to(const Variable& v) const;

	bool is_of_same_type_as(const Variable& v) const;

private:
	void *data_;
	type_t type_;
	std::string description_;
	std::vector< const ConstraintBase* > constraints_;
};


#endif // PARAMETER_HPP
