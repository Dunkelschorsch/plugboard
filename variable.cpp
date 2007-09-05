#include "variable.hpp"
#include "types.hpp"

#include <cassert>

Variable::Variable() : dims_(std::vector< uint16_t >()), type_(empty), values_(std::vector< boost::any >()) { };


Variable::Variable(std::vector< uint16_t > dimensions) : dims_(dimensions), type_(empty) { };


Variable::Variable(const
                   integer_t& value)
{
	type_ = integer;
	values_.push_back(value);
	dims_.push_back(1);
}

/** \brief Constructs a new scalar, floating point Variable object

 * \param value a constant real_t reference to initialize the variable
 *
 */
Variable::Variable(const
                   real_t& value)
{
	type_ = real;
	values_.push_back(value);
	dims_.push_back(1);
}

/** \brief Constructs a new string Variable object

 * \param value a constant string_t reference to initialize the variable
 *
 */
Variable::Variable(const
                   string_t& value)
{
	type_ = string;
	values_.push_back(value);
	dims_.push_back(1);
}

Variable::Variable(const complex_t& value)
{
	type_ = complex;
	values_.push_back(value);
	//dims_.push_back(1);
}

/** \brief Appends an integer number to an already existing Variable object

 * \param value a constant integer_t reference to append
 *
 */
void Variable::append_value(const
                            integer_t& value)
{
	if (type() == empty)
	{
		type_ = integer;
		dims_.push_back(0);
	}
	assert(type() == integer);
	values_.push_back(value);
	*(dims_.end()-1) += 1;
}


/** \brief Appends a floating point number to an already existing Variable object

 * \param value a constant real_t reference to append
 *
 */
void Variable::append_value(real_t value)
{
	if (type() == empty)
	{
		type_ = real;
		dims_.push_back(0);
	}
	assert(type() == real);
	values_.push_back(value);
	*(dims_.end()-1) += 1;
}

/** \brief Appends a complex number to an already existing Variable object

 * \param value a constant complex_t reference to append
 *
 */
void Variable::append_value(const
                            complex_t& value)
{
	if (type() == empty)
	{
		type_ = complex;
		dims_.push_back(0);
	}
	assert(type() == complex);
	values_.push_back(value);
	*(dims_.end()-1) += 1;
}


void Variable::append_value(const
                            string_t& value)
{
	if (type() == empty)
	{
		type_ = string;
		dims_.push_back(0);
	}
	assert(type() == string);
	values_.push_back(value);
	*(dims_.end()-1) ++;
}

void Variable::app(real_t value)
{
	if (type() == empty)
	{
		type_ = real;
	}
	assert(type() == real);
	values_.push_back(value);
	//*(dims_.end()-1) += 1;
}

void Variable::set_dimensions(const std::vector< uint16_t >& d)
{
	dims_ = d;
}

type_t Variable::type() const
{
	return type_;
}

std::vector< uint16_t > Variable::dims() const
{
	return dims_;
}

Variable::operator bool() const
{
	if (type() == empty)
		return false;
	else
		return true;
}

uint64_t Variable::size() const
{
	switch (type())
	{
	case (integer) :
		return sizeof(integer_t) * values_.size();
		break;
	case (real) :
		return sizeof(real_t) * values_.size();
		break;
	case (complex) :
		return sizeof(complex_t) *8*values_.size();
		break;
	case (string) :
		return sizeof(char) *8*values_.size();
		break;
	default	:
		return 0;
	}
}

std::vector<boost::any>::reference Variable::iterator::operator*() const
{
	return *it_;
}

bool Variable::iterator::operator== (const
                                     iterator& x) const
{
	return it_ == x.it_;
}

bool Variable::iterator::operator!= (const
                                     iterator& x) const
{
	return !(*this == x);
}

Variable::iterator& Variable::iterator::operator++()
{
	++it_;
	return *this;
}

Variable::iterator Variable::iterator::operator++ (int)
{
	iterator tmp = *this;
	++*this;
	return tmp;
}

Variable::iterator& Variable::iterator::operator--()
{
	--it_;
	return *this;
}

Variable::iterator Variable::iterator::operator-- (int)
{
	iterator tmp = *this;
	--*this;
	return tmp;
}

Variable::iterator Variable::iterator::insert(const
          boost::any& x)
{
	return iterator(*r_, r_->insert(it_, x));
}

Variable::iterator Variable::iterator::erase()
{
	return iterator(*r_, r_->erase(it_));
}

Variable::iterator Variable::begin()
{
	return iterator(values_, values_.begin());
}


Variable::iterator Variable::end()
{
	return iterator(values_, values_.end());
}

void Variable::add_dimension(uint16_t size)
{
	dims_.push_back(size);
}

