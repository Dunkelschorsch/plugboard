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

#include <set>

#include "grammar/command/actions.hpp"
#include "grammar/command/parser_id.hpp"
#include "exception/input.hpp"
#include "types/base.hpp"
#include "block/block.hpp"
#include "variable/variable.hpp"
#include "variable/variable_iterator.hpp"
#include "block_loader.hpp"
#include "system.hpp"

#define PB_DEBUG_MESSAGE_COLOUR \033[01;32m
#define PB_DEBUG_MESSAGE_SOURCE Parser

#include "colour_debug.hpp"

namespace plugboard
{
	boost::spirit::parser_id identify_line(const tree_iter_t& i)
	{
		return i->value.id();
	}


	template< >
	int32_t make_scalar(const tree_iter_t& i)
	{
		std::string integer(i->value.begin(), i->value.end());
		return strtol(integer.c_str(), 0, 10);
	}


	template< >
	real_t make_scalar(const tree_iter_t& i)
	{
		std::string realnum(i->value.begin(), i->value.end());
		return strtod(realnum.c_str(), 0);
	}


	template< >
	std::string make_scalar(const tree_iter_t& i)
	{
		assert(i->children.size() == 0);
		return std::string(i->value.begin(), i->value.end());
	}


	template< >
	complex_t make_scalar(const tree_iter_t& i)
	{
		std::string realnum(i->value.begin(), i->value.end());

		if(realnum == "i" || realnum == "j")
			return complex_t(0,1);
		else
			return complex_t(strtod(realnum.c_str(), 0));
	}


	type_t get_var_type(const std::string& var_name)
	{
		PB_DEBUG_MESSAGE("looking up type of " << var_name)

		return System::instance().get_variable(var_name).get_type();
	}


	type_t subtree_type(const tree_iter_t& i)
	{
		// at the leaf nodes, the type is immediately available
		if(i->value.id() == parserID::integer)
			return int32;
		if(i->value.id() == parserID::real)
			return real;
		if(i->value.id() == parserID::complex)
			return complex;
		if(i->value.id() == parserID::string)
			return string;
		if(i->value.id() == parserID::identifier)
			return get_var_type(make_scalar< std::string >(i));

		std::set< type_t > types;
		// descend further into the tree and collect types
		for(tree_iter_t branch_it = i->children.begin(); branch_it != i->children.end(); ++branch_it)
		{
			types.insert(subtree_type(branch_it));
		}

		if(types.find(string) != types.end())
			return string;
		if(types.find(complex) != types.end())
			return complex;
		if(types.find(real) != types.end())
			return real;
		if(types.find(int32) != types.end())
			return int32;

		return empty;
	}


	void eval_expression(const tree_iter_t& i, Variable& v)
	{
		if(i->value.id() == parserID::identifier)
		{
			assert(i->children.size() == 0);
			v = System::instance().get_variable(make_scalar< std::string >(i));
			return;
		}
		else
		if
		(
			i->value.id() == parserID::scalarexpression ||
			i->value.id() == parserID::scalarfactor ||
			i->value.id() == parserID::scalarterm ||
			i->value.id() == parserID::integer ||
			i->value.id() == parserID::real ||
			i->value.id() == parserID::complex
		)
		{
			type_t num_type = subtree_type(i);

			PB_DEBUG_MESSAGE("scalar arithmetic")
			PB_DEBUG_MESSAGE("type: " << num_type)

			switch(num_type)
			{
				case int32:
					v.push_back(evaluate_scalar_arithmetic< int32_t >(i));
					break;
				case real:
					v.push_back(evaluate_scalar_arithmetic< real_t >(i));
					break;
				case complex:
					v.push_back(evaluate_scalar_arithmetic< complex_t >(i));
					break;
				default:
					assert(0);
			}
		}
		else
		if(i->value.id() == parserID::array)
		{
			assert(i->children.size() > 1);

			PB_DEBUG_MESSAGE("array")
			PB_DEBUG_MESSAGE("  rows: " << i->children.size())
			PB_DEBUG_MESSAGE("  columns: " << i->children.begin()->children.size())
			PB_DEBUG_MESSAGE("  numeric type: " << subtree_type(i))

		}
		else
		if(i->value.id() == parserID::rowvec)
		{
			assert(i->children.size() > 1);

			PB_DEBUG_MESSAGE("row_vec")
			PB_DEBUG_MESSAGE("  elements: " << i->children.size())
			PB_DEBUG_MESSAGE("  numeric type: " << subtree_type(i))

			type_t num_type = subtree_type(i);

			switch(num_type)
			{
				case int32:
					make_array< int32_t >(i->children.begin(), i->children.end(), v);
					break;
				case real:
					make_array< real_t >(i->children.begin(), i->children.end(), v);
					break;
				case complex:
					make_array< complex_t >(i->children.begin(), i->children.end(), v);
					break;
				default:
					assert(0);
			}
		}
		else
		if(i->value.id() == parserID::string)
		{
			assert(i->children.size() == 0);

			v.push_back(std::string(i->value.begin(), i->value.end()));
		}
		else
		if(i->value.id() == parserID::range)
		{
			assert(i->children.size() == 3);

			PB_DEBUG_MESSAGE("range")
			PB_DEBUG_MESSAGE("  numeric type: " << subtree_type(i))

			if(subtree_type(i) == real)
			{
				make_range< real_t >
				(
					evaluate_scalar_arithmetic< real_t >(i->children.begin()),
					evaluate_scalar_arithmetic< real_t >(i->children.begin()+2),
					evaluate_scalar_arithmetic< real_t >(i->children.begin()+1),
					v
				);
			} else
			if(subtree_type(i) == int32)
			{
				make_range< int32_t >
				(
					evaluate_scalar_arithmetic< int32_t >(i->children.begin()),
					evaluate_scalar_arithmetic< int32_t >(i->children.begin()+2),
					evaluate_scalar_arithmetic< int32_t >(i->children.begin()+1),
					v
				);
			}
		}
		else assert(0);
	}


	template< typename T >
	T evaluate_scalar_arithmetic(const tree_iter_t& i)
	{
		if
		(
			(i->value.id() == parserID::integer) ||
			(i->value.id() == parserID::real) ||
			(i->value.id() == parserID::complex)
		)
		{
			assert(i->children.size() == 0);
			return make_scalar< T >(i);
		}
		else
		if(i->value.id() == parserID::scalarexpression)
		{
			if (*i->value.begin() == '+')
			{
				assert(i->children.size() == 2);
				return evaluate_scalar_arithmetic< T >(i->children.begin()) +
					evaluate_scalar_arithmetic< T >(i->children.begin()+1);
			}
			else if (*i->value.begin() == '-')
			{
				assert(i->children.size() == 2);
				return evaluate_scalar_arithmetic< T >(i->children.begin()) -
					evaluate_scalar_arithmetic< T >(i->children.begin()+1);
			}
		}
		else
		if(i->value.id() == parserID::scalarterm)
		{
			if (*i->value.begin() == '*')
			{
				assert(i->children.size() == 2);
				return evaluate_scalar_arithmetic< T >(i->children.begin()) *
					evaluate_scalar_arithmetic< T >(i->children.begin()+1);
			}
			else if (*i->value.begin() == '/')
			{
				assert(i->children.size() == 2);
				return evaluate_scalar_arithmetic< T >(i->children.begin()) /
					evaluate_scalar_arithmetic< T >(i->children.begin()+1);
			}
		}
		else
		if (i->value.id() == parserID::scalarfactor)
		{
			assert(*i->value.begin() == '-');
			return - evaluate_scalar_arithmetic< T >(i->children.begin());
		}
		assert(0);
		return 0;
	}


	template< typename T >
	void make_range(T start, T end, T inc, Variable& v)
	{
		assert(start * inc < end);

		size_t elements = static_cast< size_t >(floor((end-start) / inc +1));
		v.prealloc(sizeof(T) * (elements + v.size()));

		PB_DEBUG_MESSAGE("elements: " << elements)

		T f = start;

		for(size_t i=0; i<elements; ++i)
		{
			v.push_back(f);
			f += inc;
		}
	}


	template< typename T >
	void make_array(tree_iter_t const&  start, tree_iter_t const& end, Variable& v)
	{
		tree_iter_t curr = start;
		size_t elements = end - start;
		v.prealloc(sizeof(T) * (elements + v.size()));

		for(size_t i=0; i < elements; ++i)
		{
			v.push_back(evaluate_scalar_arithmetic< T >(curr));
			++curr;
		}
	}


	void eval_assignment(const tree_iter_t& i)
	{
		assert(i->children.size() == 2);

		std::string var_name(make_scalar< std::string >(i->children.begin()));

		PB_DEBUG_MESSAGE("assignment")
		PB_DEBUG_MESSAGE("  variable name: " << var_name)
		PB_DEBUG_MESSAGE("  data type: " << subtree_type(i->children.begin()+1))

		Variable var;
		eval_expression(i->children.begin()+1, var);
		System::instance().add_variable(var_name, var);
	}


	void eval_add_block(const tree_iter_t& i)
	{
		assert(i->children.size() >= 2);

		std::string block_name, block_type;

		uint32_t num_args = i->children.size()-2;
		block_type = make_scalar< std::string >(i->children.begin());
		block_name = make_scalar< std::string >(i->children.begin()+1);

		PB_DEBUG_MESSAGE("adding block")
		PB_DEBUG_MESSAGE("  type: " << block_type)
		PB_DEBUG_MESSAGE("  name: " << block_name)
		PB_DEBUG_MESSAGE("  no. of arguments given: " << num_args)


		plugboard::block_ptr b(plugboard::BlockLoader::instance().new_block(block_type));
		b->call_configure_parameters();

		if(b->get_params().size() != num_args)
		{
			throw ParameterCountMismatchException(block_name);
		}

		// fill block with provided parameters
		for(uint32_t param_num=0; param_num<num_args; ++param_num)
		{
			Variable param_curr;

			PB_DEBUG_MESSAGE("  parameter type: " << subtree_type(i->children.begin()+2+param_num))

			eval_expression(i->children.begin() + param_num + 2, param_curr);

			assert(subtree_type(i->children.begin()+2+param_num) == param_curr.get_type());
			assert(param_curr);

			b->set_parameter(param_curr);
		}

		assert(b->is_configured());

		System::instance().add_block(b, block_name);
	}


	void eval_connect(const tree_iter_t& i)
	{
		assert(i->children.size() == 4);

		std::string source_block_name, sink_block_name, source_port_name, sink_port_name;

		PB_DEBUG_MESSAGE("connecting blocks ...")

		source_block_name = make_scalar< std::string >(i->children.begin());
		source_port_name = make_scalar< std::string >(i->children.begin()+1);
		sink_block_name = make_scalar< std::string >(i->children.begin()+2);
		sink_port_name = make_scalar< std::string >(i->children.begin()+3);

		PB_DEBUG_MESSAGE("... " << source_block_name << ":" << source_port_name << "->"
			<< sink_block_name << ":" << sink_port_name)

		System::instance().connect_ports(source_block_name, source_port_name, sink_block_name, sink_port_name);
	}


	void eval_run(const tree_iter_t& i)
	{
		assert(i->children.size() <= 2);
		int32_t times = 1;

		if(i->children.size() == 2)
		{
			Variable v;
			eval_expression(i->children.begin()+1, v);
			if(v.get_type() == int32)
			{
				variable_iterator< int32_t > vi(v);
				times = *vi;
			}
		}

		PB_DEBUG_MESSAGE("running system " << times << " times")
		PB_DEBUG_MESSAGE("initializing...")

		System::instance().initialize();

		PB_DEBUG_MESSAGE("starting system...")

		System::instance().wakeup_sys(times);

		PB_DEBUG_MESSAGE("finalizing system...")

		System::instance().finalize();
	}


	void eval_command( const tree_iter_t & start_node )
	{
		int id = identify_line(start_node).to_long();
		switch(id)
		{
			case parserID::assignment:
				eval_assignment(start_node);
				break;
			case parserID::addblock:
				eval_add_block(start_node);
				break;
			case parserID::connect:
				eval_connect(start_node);
				break;
			case parserID::run:
				eval_run(start_node);
				break;
			default:
				assert(0);
		}
	}
} // namespace plugboard
