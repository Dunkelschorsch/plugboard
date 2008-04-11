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

#ifndef GRAMMAR_COMMAND_POLICIES_HPP
#define GRAMMAR_COMMAND_POLICIES_HPP

#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/iterator/position_iterator.hpp>

namespace plugboard
{
	using namespace boost::spirit;

	// how the input data gets to the scanner
	typedef file_iterator< char > file_iter_t;
	typedef position_iterator2< file_iter_t > iterator_t;
	// what kind of scanning is done with the input data
	typedef tree_match< iterator_t > parse_tree_match_t;
	typedef ast_match_policy< iterator_t > match_policy_t;
	// activate whitespace skpping
	typedef skip_parser_iteration_policy< space_parser > skip_parser_policy_t;

	typedef scanner_policies< skip_parser_policy_t, match_policy_t > scanner_policy_t;
	typedef scanner< iterator_t, scanner_policy_t > ast_scanner_t;
	typedef parse_tree_match_t::const_tree_iterator tree_iter_t;
} // namespace plugboard

#endif // GRAMMAR_COMMAND_POLICIES_HPP
