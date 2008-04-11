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

#ifndef GRAMMAR_COMMAND_PARSER_HPP
#define GRAMMAR_COMMAND_PARSER_HPP


namespace plugboard
{
	struct parserID
	{
		// literals
		static const int integer = 1;
		static const int real = 2;
		static const int complex = 3;
		static const int string = 4;

		static const int primitive = 5;

		// arrays
		static const int range = 6;
		static const int array = 7;
		static const int rowvec = 8;

		static const int variable = 9;

		static const int identifier = 10;

		// arithmetic
		static const int scalarfactor = 11;
		static const int scalarterm = 12;
		static const int scalarexpression = 13;
		static const int scalarpot = 14;

		static const int assignment = 50;

		// commands
		static const int command = 1000;
		static const int addblock = 1001;
		static const int connect = 1002;
		static const int run = 1003;

		// start rule
		static const int first = 10000;

		static const int openparen = 2001;
		static const int closeparen = 2002;
	};
} // namespace plugboard

#endif // GRAMMAR_COMMAND_PARSER_HPP
