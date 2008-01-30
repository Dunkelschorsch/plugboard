#include "parameter.hpp"
#include "variable/variable.hpp"
#include "constraint.hpp"


Parameter::Parameter(void * data, type_t type, const std::string & description) :
	data_(data),
	type_(type),
	description_(description),
	constraints_()
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



Parameter* Parameter::add_constraint( const ConstraintBase * c )
{
	this->constraints_.push_back(c);
	return this;
}



Parameter::~Parameter( )
{
	std::vector< const ConstraintBase* >::iterator c_it;
	for(c_it = constraints_.begin(); c_it != constraints_.end(); ++c_it)
	{
		delete *c_it;
	}
}



const std::vector< const ConstraintBase * >& Parameter::get_constraints( ) const
{
	return constraints_;
}
