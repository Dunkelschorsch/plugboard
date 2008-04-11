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

#ifndef EXCEPTION_INPUT_HPP
#define EXCEPTION_INPUT_HPP

#include "exception/base.hpp"

namespace plugboard
{
	// this one is thrown from the block library, hence the export
	class DSOEXPORT IncompatibleTypesException : public Exception< std::string >
	{
	public:
		IncompatibleTypesException(const std::string& unknownId);

		const char* do_what() const;
	};



	class ParameterCountMismatchException : public Exception< std::string >
	{
	public:
		ParameterCountMismatchException(const std::string& unknownId);

		const char* do_what() const;
	};



	class UndefinedVariableException : public Exception< std::string >
	{
	public:
		UndefinedVariableException(const std::string& unknownId);

		const char* do_what() const;
	};
}

#endif // EXCEPTION_INPUT_HPP
