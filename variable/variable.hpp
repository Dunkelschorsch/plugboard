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

#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#ifndef NDEBUG
#include <iostream>
#endif

#include <vector>
#include "visibility.hpp"
#include "types/base.hpp"
#include "types/typeinfo.hpp"
#include "pimpl/pimpl.h"


namespace plugboard
{
	class DSOEXPORT Variable : public pimpl< Variable >::pointer_semantics
	{
	public:
		Variable();
		Variable(const Variable& other);
		explicit Variable(std::vector< uint16_t > dimensions);

		template< class T >
		Variable(const T value);

		Variable& operator=(const Variable& other);
		operator bool() const;

		template< class ElementT >
		void push_back(const ElementT);

		void add_dimension(uint16_t size);

		void set_dimensions(const std::vector< uint16_t >& d);

		const std::vector< uint16_t >& get_dimensions() const;

		type_t get_type() const;

		void set_type(type_t t);

		bool save_type_change(type_t t);

		// just for the iterator. use at your own risk!
		// no. wait. don't use it at all...
		void* data() const;

		size_t size() const;

		void prealloc(size_t);
	};
} // namespace plugboard


#endif // VARIABLE_HPP
