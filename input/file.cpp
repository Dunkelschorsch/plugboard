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

#include <iostream>

#include "grammar/command/new_parse.hpp"
#include "grammar/command/policies.hpp"
#include "grammar/command/actions.hpp"

#ifndef NDEBUG
#include <boost/spirit/tree/tree_to_xml.hpp>
#include <map>
#endif

#include "input/file.hpp"
#include "exception/base.hpp"


namespace plugboard
{
	struct FileInput::FileInputImpl
	{
		command_parser parser_;
	};


	FileInput::FileInput() : d(new FileInputImpl()) { }


	FileInput::~FileInput()
	{
		delete d;
	}


	bool FileInput::execute_command(const std::string& file_name)
	{
		skip_parser_policy_t iter_policy(space_p);
		scanner_policy_t policies(iter_policy);

		// create a file_iterator
		file_iter_t file_first(file_name);
		if (!file_first)
		{
			std::cout << "Unable to open file!" << std::endl;
			// Clean up, throw an exception, whatever
			return false;
		}
		file_iter_t file_last = file_first.make_end();

		// create a position_iterator wrapping the file_iterator
		iterator_t first(file_first, file_last, file_name);
		first.set_tabchars(8);
		iterator_t last;

		ast_scanner_t scan(first, last, policies);

		try
		{
			while(first != last)
			{
				parse_tree_match_t hit = d->parser_.parse(scan);
				if (hit)
				{
#ifndef NDEBUG
					// dump ast as XML
					std::map< parser_id, std::string > rule_names;
					rule_names[parserID::integer] = "integer";
					rule_names[parserID::primitive] = "primitive";
					rule_names[parserID::real] = "realnum";
					rule_names[parserID::scalarfactor] = "scalar_factor";
					rule_names[parserID::scalarterm] = "scalar_term";
					rule_names[parserID::scalarexpression] = "scalar_expression";
					rule_names[parserID::first] = "first";
					rule_names[parserID::array] = "array";
					rule_names[parserID::rowvec] = "row_vec";
					rule_names[parserID::variable] = "variable";
					rule_names[parserID::assignment] = "assignment";
					rule_names[parserID::range] = "n_range";
					rule_names[parserID::string] = "string";
					rule_names[parserID::scalarpot] = "scalar_pot";
					rule_names[parserID::identifier] = "identifier";
					rule_names[parserID::complex] = "complex";
					rule_names[parserID::addblock] = "add_block";
					rule_names[parserID::run] = "run";
					tree_to_xml(std::cout, hit.trees, first.get_currentline(), rule_names);
#endif
					eval_command(hit.trees.begin());
				}
				else
					break;
			}
		}
		catch (boost::spirit::parser_error< parse_errors, iterator_t >& e)
		{
			file_position pos(e.where.get_position());

			if(e.descriptor == opening_paren_expected)
			{
				std::cout << pos.file << ":" << pos.line << ":" << pos.column << " " << first.get_currentline();
				std::cout << " error: »(« expected." << std::endl;
			}
			if(e.descriptor == closing_paren_expected)
			{
				std::cout << pos.file << ":" << pos.line << ":" << pos.column << " " << first.get_currentline();
				std::cout << " error: »)« expected." << std::endl;
			}
			return false;
		}
		catch (Exception< std::string >& e)
		{
			file_position pos(first.get_position());
			std::cout << file_name << ":" << pos.line << ": error: " << e.what();

			if(not e.get_id().empty())
				std::cout << ": »" << e.get_id() << "«" << std::endl;
			else
				std::cout << std::endl;

			return false;
		}
		return true;
	}
} // namespace plugboar
