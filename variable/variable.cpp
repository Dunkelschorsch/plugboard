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
#include <tr1/type_traits>
#include <limits>

#include "variable/variable.hpp"


#define PB_DEBUG_MESSAGE_COLOUR \033[22;35m
#define PB_DEBUG_MESSAGE_SOURCE Variable

#include "colour_debug.hpp"


using namespace plugboard;

template< >
struct pimpl< Variable >::implementation
{
	implementation() :
		dims(std::vector< uint16_t >()),
		numel(0),
		allocated(0),
		size(0),
		data(NULL),
		type(empty)
	{
	
	}

	implementation(const pimpl< Variable >::implementation & other);

	explicit implementation(std::vector< uint16_t > dimensions);

	// constructor for composite types
	template< class T, bool b >
	implementation(const T& value, const std::tr1::integral_constant< bool, b >&);

	// constructor for fundamental types
	template< class T >
	implementation(const T value, const std::tr1::true_type&);

	// push_back for composite types
	template< class T, bool b >
	void push_back(const T& value, const std::tr1::integral_constant< bool, b >&);

	// push_back for fundamental types
	template< class T >
	void push_back(const T value, const std::tr1::true_type&);

	~implementation();

	template< typename oldT, typename newT >
	void cast();

	void swap(implementation& other);

	template< class T >
	void copy_data_from_other(const implementation& other);

	std::vector< uint16_t > dims;

	uint64_t numel;
	uint64_t allocated;
	uint64_t size;
	void *data;
	type_t type;
};


template< class T >
void pimpl< Variable >::implementation::copy_data_from_other(const implementation& other)
{
	for(size_t i=0; i<numel; i++)
	{
		static_cast< T** >(data)[i] =
			new T(*static_cast< T** >(other.data)[i]);
	}
}


pimpl< Variable >::implementation::implementation(const pimpl< Variable >::implementation& other) :
	dims(other.dims),
	numel(other.numel),
	allocated(other.allocated),
	size(other.size),
	data(NULL),
	type(other.type)
{
	data = malloc(size);

	switch(type)
	{
	case plugboard::string:
		copy_data_from_other< string_t >(other);
		break;
	case plugboard::complex:
		copy_data_from_other< complex_t >(other);
		break;
	default:
		data = memcpy(data, other.data, size);
	}
}


template< class T >
pimpl< Variable >::implementation::implementation(const T value, const std::tr1::true_type&) :
	dims(std::vector< uint16_t >()),
	numel(1),
	allocated(1),
	size(plugboard::typeinfo< T >::size),
	data(NULL),
	type(plugboard::typeinfo< T >::value)
{
	data = malloc(size);
	static_cast< T* >(data)[0] = value;

	dims.push_back(1);
}


template< class T, bool b >
pimpl< Variable >::implementation::implementation(const T& value, const std::tr1::integral_constant< bool, b >&) :
	dims(std::vector< uint16_t >()),
	numel(1),
	allocated(1),
	size(plugboard::typeinfo< T >::size),
	data(NULL),
	type(plugboard::typeinfo< T >::value)
{
	assert(sizeof(value) == sizeof(void*));
	data = malloc(sizeof(value));

	static_cast< T** >(data)[0] = new T(value);

	dims.push_back(1);
}


pimpl< Variable >::implementation::~implementation()
{
	if(type == string)
	{
		for(size_t i=0; i<numel; ++i)
			boost::checked_delete(static_cast< string_t** >(data)[i]);
	}
	if(type == complex)
	{
		for(size_t i=0; i<numel; ++i)
			boost::checked_delete(static_cast< complex_t** >(data)[i]);
	}
	free(data);
}


template< class oldT, class newT >
struct conversion_traits
{
  inline static bool overflow_or_underflow(oldT v)
  {
    return ((v < std::numeric_limits< newT >::min()) || (v > std::numeric_limits< newT >::max()));
  }
};

template< class oldT >
struct conversion_traits< oldT, complex_t >
{
  inline static bool overflow_or_underflow(oldT v)
  {
    return false;
  }
};


template< typename oldT, typename newT >
void pimpl< Variable >::implementation::cast()
{
	void* new_data = malloc(typeinfo< newT >::size * numel);
	for(size_t i=0; i<numel; ++i)
	{
		if(conversion_traits< oldT, newT >::overflow_or_underflow(static_cast< oldT* >(data)[i]))
		{
		  throw std::exception(); //TODO come up with a proper exception here
		}
		static_cast< newT* >(new_data)[i] =
			static_cast< newT >(static_cast< oldT* >(data)[i]);
	}
	free(data);

	data = new_data;
	size = numel * typeinfo< newT >::size;

	type = typeinfo< newT >::value;
	PB_DEBUG_MESSAGE("number of variable elements: " << numel)
	PB_DEBUG_MESSAGE("new variable size (bytes): " << size)
}


template< class T, bool b >
void pimpl< Variable >::implementation::push_back(const T& value, const std::tr1::integral_constant< bool, b >&)
{
	if(type == empty)
		type = typeinfo< T >::value;

	PB_DEBUG_MESSAGE("reallocating...")

	data = realloc(data, typeinfo< T >::size * (numel+1));
	static_cast< T const ** const >(data)[numel++] = new T(value);

	size += typeinfo< T >::size;
}


template< class T >
void pimpl< Variable >::implementation::push_back(const T value, const std::tr1::true_type&)
{
	if(type == empty)
		type = typeinfo< T >::value;

	if(allocated == numel)
	{
		PB_DEBUG_MESSAGE("reallocating...")

		data = realloc(data, typeinfo< T >::size * (numel+1));
		allocated = numel+1;
	}

	static_cast< T* const >(data)[numel++] = static_cast< const T >(value);

	size += typeinfo< T >::size;
}


void pimpl< Variable >::implementation::swap(pimpl< Variable >::implementation& other)
{
	using std::swap;
	swap(this->type, other.type);
	swap(this->dims, other.dims);
	swap(this->numel, other.numel);
	swap(this->allocated, other.allocated);
	swap(this->size, other.size);
	swap(this->data, other.data);
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

	template< class T >
	Variable::Variable(const T value) : base(value, std::tr1::is_fundamental< T >())
	{
	}


	template< class ElementT >
	void Variable::push_back(const ElementT e)
	{
		implementation& impl = **this;
		impl.push_back(e, std::tr1::is_fundamental< ElementT >());
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
		(*this)->dims = d;
	}


	type_t Variable::get_type() const
	{
		return (*this)->type;
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
		return (*this)->numel;
	}


	void Variable::add_dimension(uint16_t size)
	{
		(*this)->dims.push_back(size);
	}


	void Variable::set_type(type_t t)
	{
		(*this)->type = t;
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
			PB_DEBUG_MESSAGE("converting from integer ...")

			if(new_type == real)
			{
				PB_DEBUG_MESSAGE("... to real")
				impl.cast< int32_t, real_t >();
			}
			else if(new_type == complex)
			{
				PB_DEBUG_MESSAGE("... to complex")
				impl.cast< int32_t, complex_t >();
			}
		}
		else if(old_type == real)
		{
			PB_DEBUG_MESSAGE("converting from real->")

			if(new_type == complex)
			{
				PB_DEBUG_MESSAGE("... to complex")
				impl.cast< real_t, complex_t >();
			}
		}
		return true;
	}


	const std::vector< uint16_t >& Variable::get_dimensions( ) const
	{
		return (*this)->dims;
	}


	void* Variable::data() const
	{
		return (*this)->data;
	}


	void Variable::prealloc( size_t new_size )
	{
		implementation& impl = **this;

		if(new_size > impl.allocated)
		{
			impl.data = realloc(impl.data, new_size);
			impl.allocated = new_size;
		}
	}
} // namespace plugboard

#define BOOST_PP_DEF(z, I, _) \
	template void plugboard::Variable::push_back(const CPP_TYPE(I)); \

	BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF
