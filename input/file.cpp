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
	catch (HumpException< std::string >& e)
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
