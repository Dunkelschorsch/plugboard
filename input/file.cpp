#include <iostream>
#include <boost/function.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/iterator/position_iterator.hpp>

#include "grammar/command/new_parse.hpp"

#include "input/file.hpp"
#include "exceptions.hpp"


struct HumpFile::HumpFileImpl
{
	new_command_parser parser_;
};



HumpFile::HumpFile() : d(new HumpFileImpl()) { }



HumpFile::~HumpFile()
{
	delete d;
}



bool HumpFile::execute_command(const std::string& file_name)
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
			rule_names[new_command_parser::integerID] = "integer";
			rule_names[new_command_parser::primitiveID] = "primitive";
			rule_names[new_command_parser::realID] = "realnum";
			rule_names[new_command_parser::scalarfactorID] = "scalar_factor";
			rule_names[new_command_parser::scalartermID] = "scalar_term";
			rule_names[new_command_parser::scalarexpressionID] = "scalar_expression";
			rule_names[new_command_parser::firstID] = "first";
			rule_names[new_command_parser::arrayID] = "array";
			rule_names[new_command_parser::rowvecID] = "row_vec";
			rule_names[new_command_parser::variableID] = "variable";
			rule_names[new_command_parser::assignmentID] = "assignment";
			rule_names[new_command_parser::rangeID] = "n_range";
			rule_names[new_command_parser::stringID] = "string";
			rule_names[new_command_parser::scalarpotID] = "scalar_pot";
			rule_names[new_command_parser::identifierID] = "identifier";
			rule_names[new_command_parser::complexID] = "complex";
			rule_names[new_command_parser::addblockID] = "add_block";
			rule_names[new_command_parser::runID] = "run";
			tree_to_xml(std::cout, hit.trees, first.get_currentline(), rule_names);
#endif
				if(identify_line(hit.trees.begin()) == new_command_parser::assignmentID)
				{
					eval_assignment(hit.trees.begin());
				}
				else if(identify_line(hit.trees.begin()) == new_command_parser::addblockID)
				{
					eval_add_block(hit.trees.begin());
				}
				else if(identify_line(hit.trees.begin()) == new_command_parser::connectID)
				{
					eval_connect(hit.trees.begin());
				}
				else if(identify_line(hit.trees.begin()) == new_command_parser::runID)
				{
					eval_run(hit.trees.begin());
				}
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
			std::cout << " error: '(' expected." << std::endl;
		}
		if(e.descriptor == closing_paren_expected)
		{
			std::cout << pos.file << ":" << pos.line << ":" << pos.column << " " << first.get_currentline();
			std::cout << " error: ')' expected." << std::endl;
		}
		return false;
	}
	catch (InvalidBlockIdException& e)
	{
		std::cout << e.what() << ": " << e.get_id() << std::endl;
		return false;
	}

	if(first != last)
	{
		file_position pos(first.get_position());
		std::cout << "Syntax error in input file!" << std::endl;
		std::cout << pos.file << ":" << pos.line << " " << first.get_currentline() << std::endl;
		return false;
	}
	return true;
}
