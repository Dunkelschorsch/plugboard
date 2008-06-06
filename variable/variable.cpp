/*!
 * \file
 * \brief
 * \author Armin Schmidt
 *
 * ----------------------------------------------------------------------------
 *
 * PlugBoard - A versatile communication simulation framework
 * Copyright (C) 2007-2008  Armin Schmidt
 *
 * This file is part of PlugBoard.
 *
 * PlugBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PlugBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PlugBoard.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ----------------------------------------------------------------------------
 */

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <algorithm>

#include "variable/variable.hpp"

#ifndef NDEBUG
#include <iostream>
#endif

using namespace plugboard;

template< >
struct pimpl< Variable >::implementation
{
	implementation() :
		dims_(std::vector< uint16_t >()),
		numel_(0),
		allocated_(0),
		size_(0),
		data_(NULL),
		type_(empty)
	{
	
	}

	implementation(int32_t value);
	implementation(real_t value);
	implementation(const string_t& value);
	implementation(const complex_t& value);
	implementation(std::vector< uint16_t > dimensions);
	implementation(const pimpl< Variable >::implementation & other);

	~implementation();

	template< typename oldT, typename newT >
	void cast();

	void swap(implementation& other);

	std::vector< uint16_t > dims_;

	uint64_t numel_;
	uint64_t allocated_;
	uint64_t size_;
	void *data_;
	type_t type_;
};


pimpl< Variable >::implementation::~implementation()
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


pimpl< Variable >::implementation::implementation(const pimpl< Variable >::implementation& other) :
	dims_(other.dims_),
	numel_(other.numel_),
	allocated_(other.allocated_),
	size_(other.size_),
	data_(NULL),
	type_(other.type_)
{
	data_ = malloc(size_);

	if(type_ == plugboard::string)
	{
		for(size_t i=0; i<numel_; ++i)
		{
			static_cast< string_t** >(data_)[i] =
				new string_t(*static_cast< string_t** >(other.data_)[i]);
		}
	}
	else
	if(type_ == plugboard::complex)
	{
		for(size_t i=0; i<numel_; ++i)
		{
			static_cast< complex_t** >(data_)[i] =
				new complex_t(*static_cast< complex_t** >(other.data_)[i]);
		}
	}
	else
	{
		data_ = memcpy(data_, other.data_, size_);
	}
}


pimpl< Variable >::implementation::implementation(int32_t value) :
	dims_(std::vector< uint16_t >()),
	numel_(1),
	allocated_(1),
	size_(sizeof(int32_t)),
	data_(NULL),
	type_(int32)
{
	data_ = malloc(sizeof(int32_t));
	static_cast< int32_t* >(data_)[0] = value;

	dims_.push_back(1);
	dims_.push_back(1);
}


pimpl< Variable >::implementation::implementation(real_t value) :
	dims_(std::vector< uint16_t >()),
	numel_(1),
	allocated_(1),
	size_(sizeof(real_t)),
	data_(NULL),
	type_(real)
{
	data_ = malloc(sizeof(real_t));
	static_cast< real_t* >(data_)[0] = value;

	dims_.push_back(1);
	dims_.push_back(1);
}


pimpl< Variable >::implementation::implementation(const string_t& value) :
	dims_(std::vector< uint16_t >()),
	numel_(1),
	allocated_(1),
	size_(sizeof(string_t)),
	data_(NULL),
	type_(plugboard::string)
{
	assert(sizeof(value) == sizeof(void*));
	data_ = malloc(sizeof(value));

	static_cast< string_t** >(data_)[0] = new string_t(value);

	dims_.push_back(1);
	dims_.push_back(1);
}


pimpl< Variable >::implementation::implementation(const complex_t& value) :
	dims_(std::vector< uint16_t >()),
	numel_(1),
	allocated_(1),
	size_(sizeof(complex_t)),
	data_(NULL),
	type_(complex)
{
	assert(sizeof(value) == sizeof(void*));
	data_ = malloc(sizeof(value));

	static_cast< complex_t** >(data_)[0] = new complex_t(value);

	dims_.push_back(1);
	dims_.push_back(1);
}


template< typename oldT, typename newT >
void pimpl< Variable >::implementation::cast()
{
	void* new_data = malloc(typeinfo< newT >::size * numel_);
	for(size_t i=0; i<numel_; ++i)
	{
		static_cast< newT* >(new_data)[i] =
			static_cast< newT >(static_cast< oldT* >(data_)[i]);
	}
	free(data_);

	data_ = new_data;
	size_ = numel_ * typeinfo< newT >::size;

	type_ = typeinfo< newT >::value;
#ifndef NDEBUG
	std::cout << "[Variable] number of variable elements: " << numel_ << std::endl;
	std::cout << "[Variable] new variable size (bytes): " << size_ << std::endl;
#endif
}


void pimpl< Variable >::implementation::swap(pimpl< Variable >::implementation& other)
{
	using std::swap;
	swap(this->type_, other.type_);
	swap(this->dims_, other.dims_);
	swap(this->numel_, other.numel_);
	swap(this->allocated_, other.allocated_);
	swap(this->size_, other.size_);
	swap(this->data_, other.data_);
}


namespace plugboard
{
	using std::malloc;
	using std::realloc;
	using std::free;
	using std::memcpy;


	Variable::Variable() : base()
	{
	}

	Variable::Variable(const Variable& other) : base(other)
	{
	}

	Variable::Variable(int32_t value) : base(value)
	{
	}

	/** \brief Constructs a new scalar, floating point Variable object

	* \param value a constant real_t reference to initialize the variable
	*
	*/
	Variable::Variable(real_t value) : base(value)
	{
	}

	/** \brief Constructs a new string Variable object

	* \param value a constant string_t reference to initialize the variable
	*
	*/
	Variable::Variable(const string_t& value) : base(value)
	{
	}

	Variable::Variable(const complex_t& value) : base(value)
	{
	}


	template< class ElementT >
	void Variable::push_back(const ElementT e)
	{
		implementation& impl = **this;

		if(impl.type_ == empty)
			impl.type_ = typeinfo< ElementT >::value;

		if(impl.allocated_ == impl.numel_)
		{
#ifndef NDEBUG
			std::cout << "[Variable] reallocating..." << std::endl;
#endif
			impl.data_ = realloc(impl.data_, typeinfo< ElementT >::size * (impl.numel_+1));
			impl.allocated_ = impl.numel_+1;
		}

		static_cast< ElementT* >(impl.data_)[impl.numel_++] = static_cast< const ElementT >(e);

		impl.size_ += typeinfo< ElementT >::size;
	}


	template< >
	void Variable::push_back(const string_t e)
	{
		implementation& impl = **this;

		if(impl.type_ == empty)
			impl.type_ = typeinfo< string_t >::value;

		impl.data_ = realloc(impl.data_, typeinfo< string_t >::size * (impl.numel_+1));
		static_cast< string_t** >(impl.data_)[impl.numel_++] = new string_t(e);

		impl.size_ += typeinfo< string_t >::size;
	}


	template< >
	void Variable::push_back(const complex_t e)
	{
		implementation& impl = **this;

		if(impl.type_ == empty)
			impl.type_ = typeinfo< complex_t >::value;

		impl.data_ = realloc(impl.data_, typeinfo< complex_t >::size * (impl.numel_+1));
		static_cast< complex_t** >(impl.data_)[impl.numel_++] = new complex_t(e);

		impl.size_ += typeinfo< complex_t >::size;
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
		(*this)->dims_ = d;
	}


	type_t Variable::get_type() const
	{
		return (*this)->type_;
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
		return (*this)->numel_;
	}


	void Variable::add_dimension(uint16_t size)
	{
		(*this)->dims_.push_back(size);
	}


	void Variable::set_type(type_t t)
	{
		(*this)->type_ = t;
	}


	bool Variable::save_type_change(type_t new_type)
	{
		implementation& impl = **this;
		assert(new_type != empty);

		type_t old_type = get_type();

		if(new_type < old_type)
			return false;
		else if(new_type == old_type)
			return true;

		if(old_type == int32)
		{
#ifndef NDEBUG
			std::cout << "[Variable] converting integer->";
#endif
			if(new_type == real)
			{
#ifndef NDEBUG
				std::cout << "real" << std::endl;
#endif
				impl.cast< int32_t, real_t >();
			}
			else if(new_type == complex)
			{
#ifndef NDEBUG
				std::cout << "complex" << std::endl;
#endif
				impl.cast< int32_t, complex_t >();
			}
		}
		else if(old_type == real)
		{
#ifndef NDEBUG
			std::cout << "[Variable] converting real->";
#endif
			if(new_type == complex)
			{
#ifndef NDEBUG
				std::cout << "complex" << std::endl;
#endif
				impl.cast< real_t, complex_t >();
			}
		}
		return true;
	}


	const std::vector< uint16_t >& Variable::get_dimensions( ) const
	{
		return (*this)->dims_;
	}


	void* Variable::data() const
	{
		return (*this)->data_;
	}


	void Variable::prealloc( size_t new_size )
	{
		implementation& impl = **this;

		if(new_size > impl.allocated_)
		{
			impl.data_ = realloc(impl.data_, new_size);
			impl.allocated_ = new_size;
		}
	}
} // namespace plugboard

#define BOOST_PP_DEF(z, I, _) \
	template void plugboard::Variable::push_back(const CPP_TYPE(I));

	BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF
