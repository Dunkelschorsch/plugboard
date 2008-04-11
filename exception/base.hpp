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

#ifndef EXCEPTION_BASE_HPP
#define EXCEPTION_BASE_HPP


#include <stdexcept>
#include "visibility.hpp"

namespace plugboard
{
	template < class IdentifierType >
	class DSOEXPORT Exception : public std::exception
	{
	public:
		Exception(const IdentifierType& unknownId)
			: unknownId_(unknownId) { }

		const char* what() const throw()
		{
			return do_what();
		}

		const IdentifierType get_id() const throw()
		{
			return unknownId_;
		};

		virtual ~Exception() throw() { };

	private:
		IdentifierType unknownId_;

		virtual const char* do_what() const = 0;
	};
}

#endif // EXCEPTION_BASE_HPP
