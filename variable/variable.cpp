#include <cstdlib>
#include <cstring>
#include <cassert>
#include <algorithm>

#include "variable/variable.hpp"

#ifndef NDEBUG
#include <iostream>
#endif

using std::malloc;
using std::realloc;
using std::free;
using std::memcpy;



Variable::Variable() :
	dims_(std::vector< uint16_t >()),
	numel_(0),
	allocated_(0),
	size_(0),
	data_(NULL),
	type_(empty)
{
}



Variable::Variable(const Variable & other) :
	dims_(other.dims_),
	numel_(other.numel_),
	allocated_(other.allocated_),
	size_(other.size_),
	data_(NULL),
	type_(other.type_)
{
	this->data_ = malloc(this->size_);

	if(type_ == string)
	{
		for(size_t i=0; i<numel_; ++i)
		{
			static_cast< string_t** >(data_)[i] =
				new string_t(*static_cast< string_t** >(other.data_)[i]);
		}
	} else
	if(type_ == complex)
	{
		for(size_t i=0; i<numel_; ++i)
		{
			static_cast< complex_t** >(data_)[i] =
				new complex_t(*static_cast< complex_t** >(other.data_)[i]);
		}
	} else
	this->data_ = memcpy(this->data_, other.data_, this->size_);
}



Variable::Variable(int32_t value) :
	dims_(std::vector< uint16_t >()),
	numel_(1),
	allocated_(1),
	size_(sizeof(int32_t)),
	data_(NULL),
	type_(empty)
{
	type_ = int32;
	data_ = malloc(sizeof(int32_t));
	static_cast< int32_t* >(data_)[0] = value;

	dims_.push_back(1);
	dims_.push_back(1);
}



/** \brief Constructs a new scalar, floating point Variable object

 * \param value a constant real_t reference to initialize the variable
 *
 */
Variable::Variable(real_t value) :
	dims_(std::vector< uint16_t >()),
	numel_(1),
	allocated_(1),
	size_(sizeof(real_t)),
	data_(NULL),
	type_(empty)
{
	type_ = real;
	data_ = malloc(sizeof(real_t));
	static_cast< real_t* >(data_)[0] = value;

	dims_.push_back(1);
	dims_.push_back(1);
}



/** \brief Constructs a new string Variable object

 * \param value a constant string_t reference to initialize the variable
 *
 */
Variable::Variable(const string_t& value) :
	dims_(std::vector< uint16_t >()),
	numel_(1),
	allocated_(1),
	size_(sizeof(string_t)),
	data_(NULL),
	type_(empty)
{
	type_ = string;
	assert(sizeof(value) == sizeof(void*));
	data_ = malloc(sizeof(value));

	static_cast< string_t** >(data_)[0] = new string_t(value);

	dims_.push_back(1);
	dims_.push_back(1);
}



Variable::Variable(const complex_t& value) :
	dims_(std::vector< uint16_t >()),
	numel_(1),
	allocated_(1),
	size_(sizeof(complex_t)),
	data_(NULL),
	type_(empty)
{
	type_ = complex;
	assert(sizeof(value) == sizeof(void*));
	data_ = malloc(sizeof(value));

	static_cast< complex_t** >(data_)[0] = new complex_t(value);

	dims_.push_back(1);
	dims_.push_back(1);
}


template< >
void Variable::push_back(const string_t e)
{
	if(type_ == empty)
		type_ = typeinfo< string_t >::value;

	data_ = realloc(data_, typeinfo< string_t >::size * (numel_+1));
	static_cast< string_t** >(data_)[numel_++] = new string_t(e);

	size_ += typeinfo< string_t >::size;
}


template< >
void Variable::push_back(const complex_t e)
{
	if(type_ == empty)
		type_ = typeinfo< complex_t >::value;

	data_ = realloc(data_, typeinfo< complex_t >::size * (numel_+1));
	static_cast< complex_t** >(data_)[numel_++] = new complex_t(e);

	size_ += typeinfo< complex_t >::size;
}



Variable & Variable::operator =(const Variable & other)
{
	if(this != &other)
	{
		Variable tmp(other);
		swap(tmp);
	}
	return *this;
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



size_t Variable::size() const
{
	return numel_;
}



void Variable::add_dimension(uint16_t size)
{
	dims_.push_back(size);
}



void Variable::set_type(type_t t)
{
	type_ = t;
}



bool Variable::save_type_change(type_t new_type)
{
	assert(new_type != empty);

	type_t old_type = this->type_;

	if(new_type < old_type)
		return false;
	else if(new_type == old_type)
		return true;

	if(old_type == int32)
	{
#ifndef NDEBUG
		std::cout << "converting integer->";
#endif
		if(new_type == real)
		{
#ifndef NDEBUG
			std::cout << "real" << std::endl;
#endif
			cast< int32_t, real_t >();
		}
		else if(new_type == complex)
		{
#ifndef NDEBUG
			std::cout << "complex" << std::endl;
#endif
			cast< int32_t, complex_t >();
		}
	}
	else if(old_type == real)
	{
#ifndef NDEBUG
		std::cout << "converting real->";
#endif
		if(new_type == complex)
		{
#ifndef NDEBUG
			std::cout << "complex" << std::endl;
#endif
			cast< real_t, complex_t >();
		}
	}
	return true;
}



const std::vector< uint16_t >& Variable::get_dimensions( ) const
{
	return dims_;
}



Variable::~ Variable()
{
	if(type_ == string)
	{
		for(size_t i=0; i<numel_; ++i)
			delete static_cast< string_t** >(data_)[i];
	}
	if(type_ == complex)
	{
		for(size_t i=0; i<numel_; ++i)
			delete static_cast< complex_t** >(data_)[i];
	}
	free(data_);
}



void* Variable::data() const
{
	return data_;
}



template< typename oldT, typename newT >
void Variable::cast()
{
	void* new_data = malloc(typeinfo< newT >::size * this->numel_);
	for(size_t i=0; i<this->numel_; ++i)
	{
		static_cast< newT* >(new_data)[i] =
			static_cast< newT >(static_cast< oldT* >(this->data_)[i]);
	}
	free(this->data_);
	this->data_ = new_data;

	size_ = numel_ * typeinfo< newT >::size;

	type_ = typeinfo< newT >::value;
#ifndef NDEBUG
	std::cout << "number of variable elements: " << numel_ << std::endl;
	std::cout << "new variable size (bytes): " << size_ << std::endl;
#endif
}



void Variable::swap(Variable & other)
{
	using std::swap;
	swap(this->type_, other.type_);
	swap(this->dims_, other.dims_);
	swap(this->numel_, other.numel_);
	swap(this->allocated_, other.allocated_);
	swap(this->size_, other.size_);
	swap(this->data_, other.data_);
}



void Variable::prealloc( size_t new_size )
{
	if(new_size > allocated_)
	{
		data_ = realloc(data_, new_size);
		allocated_ = new_size;
	}
}
