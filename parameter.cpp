#include "parameter.hpp"
#include "variable/variable.hpp"


Parameter::Parameter(void * data, type_t type, const std::string & description) :
	data_(data),
	type_(type),
	description_(description)
{
}



type_t Parameter::get_type() const
{
	return type_;
}



bool Parameter::is_of_same_type_as(const Variable & v) const
{
	return this->get_type() == v.get_type();
}



bool Parameter::is_convertible_to(const Variable & v) const
{
	return this->get_type() >= v.get_type();
}



void * Parameter::get_data() const
{
	return data_;
}



const std::string & Parameter::get_description() const
{
	return description_;
}
