#ifndef _PARAMETER_HPP
#define _PARAMETER_HPP

#include "types.hpp"
class Variable;


class Parameter
{
public:
	Parameter(void* data, type_t type, const std::string& description);

	type_t get_type() const;

	void *get_data();

	const std::string& get_description() const;

	bool is_convertible_to(const Variable& v) const;

	bool is_of_same_type_as(const Variable& v) const;

protected:
	void *data_;
	type_t type_;
	std::string description_;
};


#endif // _PARAMETER_HPP
