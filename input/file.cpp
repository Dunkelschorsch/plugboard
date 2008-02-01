#include <iostream>
#include <boost/function.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/iterator/position_iterator.hpp>

#include "grammar/command/command_parse.hpp"
#include "input/file.hpp"
#include "exceptions.hpp"


struct HumpFile::HumpFileImpl
{
	HumpFileImpl();

	arg_storage_t v;
	CommandParser parser_;
};



HumpFile::HumpFileImpl::HumpFileImpl() : parser_(v) { }



HumpFile::HumpFile()
{
	d = new HumpFileImpl();
}



HumpFile::~HumpFile()
{
	delete d;
}



bool HumpFile::execute_command(const std::string& file_name)
{
	typedef file_iterator< char > file_iter_t;
	typedef position_iterator2< file_iter_t > iterator_t;
	typedef skip_parser_iteration_policy< space_parser > iter_policy_t;
	typedef scanner_policies< iter_policy_t > scanner_policies_t;
	typedef scanner< iterator_t, scanner_policies_t > scanner_t;
	typedef rule< scanner_t > rule_t;

	iter_policy_t iter_policy(space_p);
	scanner_policies_t policies(iter_policy);

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

	scanner_t scan(first, last, policies);

	boost::function< void() > f;
	try
	{
		while(first != last && d->parser_[::phoenix::var(f)=::phoenix::arg1].parse(scan))
		{
			f();
		}
	}
	catch (HumpException< std::string >& e)
	{
		std::cout << "exception raised. cause: " << e.what() << std::endl;
		return false;
	}


	if(first != last)
	{
		file_position pos(first.get_position());
		std::cout << "Syntax error in input file!" << std::endl;
		std::cout << pos.file << ":" << pos.line << ":" << pos.column << " " << first.get_currentline() << std::endl;
		return false;
	}
	return true;
}
