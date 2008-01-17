#include <iostream>
#include <boost/function.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>

#include "input/command_parse.hpp"
#include "input/file.hpp"



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
	typedef char char_t;
	typedef file_iterator< char_t > iterator_t;
	typedef skip_parser_iteration_policy< space_parser > iter_policy_t;
	typedef scanner_policies< iter_policy_t > scanner_policies_t;
	typedef scanner< iterator_t, scanner_policies_t > scanner_t;
	typedef rule< scanner_t > rule_t;

	iter_policy_t iter_policy(space_p);
	scanner_policies_t policies(iter_policy);

	iterator_t first(file_name);
	if (!first)
	{
		std::cout << "Unable to open file!" << std::endl;
		// Clean up, throw an exception, whatever
		return false;
	}
	iterator_t last = first.make_end();

	scanner_t scan(first, last, policies);

	boost::function< void() > f;
	while(first != last && d->parser_[::phoenix::var(f)=::phoenix::arg1].parse(scan))
	{
		f();
	}

	if(first != last)
	{
		std::cout << "-------------------------" << std::endl;
		std::cout << "Parsing failed"            << std::endl;
		std::cout << "stopped at: \""            << *first << "\"" << std::endl;
		std::cout << "-------------------------" << std::endl;
		return false;
	}
	return true;
}
