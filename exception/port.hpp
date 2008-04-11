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

#ifndef EXCEPTION_PORT_HPP
#define EXCEPTION_PORT_HPP

#include "exception/base.hpp"

namespace plugboard
{
	class InvalidPortNameException : public Exception< std::string >
	{
	public:
		InvalidPortNameException(const std::string& unknownId);

		const char* do_what() const;
	};


	class DuplicatePortNameException : public Exception< std::string >
	{
	public:
		DuplicatePortNameException(const std::string& unknownId);

		const char* do_what() const;
	};
}

#endif // EXCEPTION_PORT_HPP
