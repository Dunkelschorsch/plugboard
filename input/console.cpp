#include <algorithm>
#include <iostream>
#include <boost/function.hpp>

#include "command_parse.hpp"
#include "console.hpp"
#include "block_loader.hpp"

#include "swift/SReadline.h"


struct HumpShell::HumpShellImpl
{
	HumpShellImpl(System & sys, BlockLoader & bl);

	void register_completions();

	void set_available_blocks(const BlockLoader& bl);

	void add_available_blocks(const BlockLoader& bl);

	std::string prompt_;
	swift::SReadline reader_;
	std::vector< std::string > available_blocks_;
	std::vector< std::string > completers_;

	std::vector< boost::any > v;
	CommandParser p_;
	parse_info<> info_;

	System & sys_;
	BlockLoader & bl_;
};



HumpShell::HumpShellImpl::HumpShellImpl(System & sys, BlockLoader & bl) :
	prompt_(">> "),
	reader_("hump_history", 256),
	completers_(),
	p_(v),
	sys_(sys),
	bl_(bl)
{

}


HumpShell::HumpShell(System & s, BlockLoader & bl)
{
	d = new HumpShellImpl(s, bl);
	d->add_available_blocks(bl);
}



HumpShell::~HumpShell()
{
	delete d;
}



void HumpShell::HumpShellImpl::add_available_blocks(const BlockLoader & bl)
{
	std::copy
	(
		bl.available_blocks().begin(),
		bl.available_blocks().end(),
		back_inserter(available_blocks_)
	);
	register_completions();
}



void HumpShell::HumpShellImpl::set_available_blocks(const BlockLoader & bl)
{
	available_blocks_ = bl.available_blocks();
	register_completions();
}



void HumpShell::HumpShellImpl::register_completions()
{
	completers_.clear();

	// self-explanatory ?
	completers_.push_back("quit");

	for
	(
		std::vector< std::string >::const_iterator it = available_blocks_.begin();
		it != available_blocks_.end();
		++it
	)
	{
		// add completers for the "add block" command
		completers_.push_back(hump::add_block_command + " " + *it);
		// add completers for the "describe block" command
		completers_.push_back(hump::describe_block_command + " " + *it);
	}

	reader_.RegisterCompletions(completers_);
}



const std::string HumpShell::read_command()
{
	swift::SReadline dirty_hack__;
	std::string command_input;
	bool end_of_input(false);

	command_input = d->reader_.GetLine(d->prompt_, end_of_input);

	if (end_of_input)
	{
		std::cout << "quit" << std::endl;
		return std::string("quit");
	}
	else
	{
		return command_input;
	}
}



bool HumpShell::execute_command()
{
	swift::SReadline dirty_hack__;
	boost::function< void(System&, BlockLoader&) > f;

	std::string command_string = read_command();

	if(command_string == "quit")
	{
		return false;
	}

	using namespace phoenix;
	d->info_ = parse(command_string.c_str(), d->p_[var(f)=arg1], space_p);

	if (d->info_.full)
	{
		f(d->sys_, d->bl_);
	}
	else
	{
		std::cout << "-------------------------\n";
		std::cout << "Parsing failed\n";
		std::cout << "stopped at: \"" << d->info_.stop << "\"\n";
		std::cout << "-------------------------\n";
	}
	return true;
}
