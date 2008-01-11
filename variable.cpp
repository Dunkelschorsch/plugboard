#include "types.hpp"
#include "variable.hpp"
#include <iostream>
#include <boost/any.hpp>



Variable::Variable() :
	dims_(std::vector< uint16_t >()),
	type_(empty),
	values_(std::vector< boost::any >())
{
}



Variable::Variable(const integer_t& value)
{
	type_ = integer;
	values_.push_back(value);
	dims_.push_back(1);
	dims_.push_back(1);
}



/** \brief Constructs a new scalar, floating point Variable object

 * \param value a constant real_t reference to initialize the variable
 *
 */
Variable::Variable(const real_t& value)
{
	type_ = real;
	values_.push_back(value);
	dims_.push_back(1);
	dims_.push_back(1);
}



/** \brief Constructs a new string Variable object

 * \param value a constant string_t reference to initialize the variable
 *
 */
Variable::Variable(const string_t& value)
{
	type_ = string;
	values_.push_back(value);
	dims_.push_back(1);
	dims_.push_back(1);
}



Variable::Variable(const complex_t& value)
{
	type_ = complex;
	values_.push_back(value);
	dims_.push_back(1);
	dims_.push_back(1);
}



void Variable::append(const boost::any& value)
{
	values_.push_back(value);
}



void Variable::set_dimensions(const std::vector< uint16_t >& d)
{
	dims_ = d;
}



type_t Variable::get_type() const
{
	return type_;
}



Variable::operator bool() const
{
	if (get_type() == empty)
		return false;
	else
		return true;
}



uint64_t Variable::size() const
{
	switch (get_type())
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



Variable::iterator Variable::iterator::operator++(int)
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



void Variable::set_type(const type_t t)
{
	type_ = t;
}



struct ChangeTypeAction
{
	ChangeTypeAction(const type_t& old_type, const type_t& new_type) : old_(old_type), new_(new_type) { }

	template< typename ElementT >
	void operator()(ElementT& e)
	{
		if(old_ == integer)
		{
#ifndef NDEBUG
			std::cout << "converting integer->";
#endif
			if(new_ == real)
			{
#ifndef NDEBUG
				std::cout << "real" << std::endl;
#endif
				e = static_cast< real_t >(boost::any_cast< integer_t >(e));
			}
			else if(new_ == complex)
			{
#ifndef NDEBUG
				std::cout << "complex" << std::endl;
#endif
				e = static_cast< complex_t >(boost::any_cast< integer_t >(e));
			}
		}
		else if(old_ == real)
		{
#ifndef NDEBUG
			std::cout << "converting real->";
#endif
			if(new_ == complex)
			{
#ifndef NDEBUG
				std::cout << "complex" << std::endl;
#endif
				e = static_cast< complex_t >(boost::any_cast< real_t >(e));
			}
		}
	}

	const type_t &old_, &new_;

	typedef void reslt_type;
};



void Variable::save_type_change(const type_t t)
{
	// we change
	if(t > this->get_type())
	{
		try
		{
			std::for_each
			(
				values_.begin(),
				values_.end(),
				ChangeTypeAction(this->get_type(), t)
			);
		}
		catch(boost::bad_any_cast &e) { };

		type_ = t;
	}
}



const std::vector< uint16_t >& Variable::get_dimensions( ) const
{
	return dims_;
}
