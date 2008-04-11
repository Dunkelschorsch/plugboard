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

#ifndef GRAMMAR_COMMAND_ACTION_HPP
#define GRAMMAR_COMMAND_ACTION_HPP

#include "grammar/command/policies.hpp"
#include "types/base.hpp"

namespace plugboard
{
	class Variable;

	boost::spirit::parser_id identify_line(const tree_iter_t& i);

	type_t subtree_type(const tree_iter_t& i);

	type_t get_var_type(const std::string& var_name);

	void eval_expression(const tree_iter_t& i, Variable& v);

	template< typename T >
	T make_scalar(const tree_iter_t& i);

	template< typename T >
	void make_range(T start, T end, T inc, Variable& v);

	template< typename T >
	void make_array(tree_iter_t const&  start, tree_iter_t const& end, Variable& v);

	template< typename T >
	T evaluate_scalar_arithmetic(const tree_iter_t& i);

	void eval_assignment(const tree_iter_t& i);

	void eval_add_block(const tree_iter_t& i);

	void eval_connect(const tree_iter_t& i);

	void eval_run(const tree_iter_t& i);

	void eval_command(const tree_iter_t& i);
} // namespace plugboard

#endif // GRAMMAR_COMMAND_ACTION_HPP
