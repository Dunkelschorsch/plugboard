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

#include "exception/signal.hpp"
#include "exception/block.hpp"
#include "exception/port.hpp"
#include "exception/input.hpp"

namespace plugboard
{
	InvalidBlockNameException::InvalidBlockNameException(const std::string& unknownId)
		: Exception< std::string >(unknownId)
	{ }


	const char* InvalidBlockNameException::do_what() const
	{
		return "invalid block name passed";
	}


	InvalidBlockIdException::InvalidBlockIdException(const std::string& unknownId)
		: Exception< std::string >(unknownId)
	{ }


	const char* InvalidBlockIdException::do_what() const
	{
		return "invalid block ID passed";
	}


	InvalidPortNameException::InvalidPortNameException(const std::string& unknownId)
		: Exception< std::string >(unknownId)
	{ }


	const char* InvalidPortNameException::do_what() const
	{
		return "invalid port name passed";
	}


	BlockNotConfiguredException::BlockNotConfiguredException(const std::string& unknownId)
		: Exception< std::string >(unknownId)
	{ }

	const char* BlockNotConfiguredException::do_what() const
	{
		return "block is not configured";
	}


	DuplicatePortNameException::DuplicatePortNameException(const std::string& unknownId)
		: Exception< std::string >(unknownId)
	{ }

	const char* DuplicatePortNameException::do_what() const
	{
		return "port name already exists";
	}


	DuplicateBlockNameException::DuplicateBlockNameException(const std::string& unknownId)
		: Exception< std::string >(unknownId)
	{ }

	const char* DuplicateBlockNameException::do_what() const
	{
		return "block name already exists";
	}


	SampleTimesMismatchException::SampleTimesMismatchException(const std::string& unknownId)
		: Exception< std::string >(unknownId)
	{ }

	const char* SampleTimesMismatchException::do_what() const
	{
		return "sample times of ports do not match";
	}


	SignalTypesMismatchException::SignalTypesMismatchException(const std::string& unknownId)
		: Exception< std::string >(unknownId)
	{ }

	const char* SignalTypesMismatchException::do_what() const
	{
		return "signal types of ports do not match";
	}


	FrameSizesMismatchException::FrameSizesMismatchException(const std::string& unknownId)
		: Exception< std::string >(unknownId)
	{ }

	const char* FrameSizesMismatchException::do_what() const
	{
		return "frame sizes of ports do not match";
	}


	IncompatibleTypesException::IncompatibleTypesException(const std::string& unknownId)
		: Exception< std::string >(unknownId)
	{ }

	const char* IncompatibleTypesException::do_what() const
	{
		return "incompatible types in assignement";
	}


	ParameterCountMismatchException::ParameterCountMismatchException(const std::string& unknownId)
		: Exception< std::string >(unknownId)
	{ }

	const char* ParameterCountMismatchException::do_what() const
	{
		return "wrong number of parameters for block";
	}


	UndefinedVariableException::UndefinedVariableException(const std::string& unknownId)
		: Exception< std::string >(unknownId)
	{ }

	const char* UndefinedVariableException::do_what() const
	{
		return "variable not defined";
	}
} // namesopace plugboard
