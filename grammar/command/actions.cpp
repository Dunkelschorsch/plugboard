#include <cassert>
#include <iostream>

#include "exceptions.hpp"
#include "actions.hpp"
#include "block/block.hpp"
#include "block_loader.hpp"
#include "system.hpp"
#include "systems.hpp"
#include "variable/variable.hpp"



VariableLookupAction::VariableLookupAction(arg_storage_t& args) : args_(args) { }

void VariableLookupAction::operator()(const std::string& var_name) const
{
	arg_storage_t::const_iterator arg_iter = args_.begin();
#ifndef NDEBUG
	std::cout << "Variable to look up: '" << var_name << "'." << std::endl;
#endif
	const Variable& vv = Systems::instance().get_root()->get_variable(var_name);
	assert(vv == true);
	args_.push_back(vv);
}



namespace commands
{

BlockAddCommand::BlockAddCommand(const arg_storage_t& args) : args_(args) { }

BlockAddCommand::result_type BlockAddCommand::operator()() const
{
	arg_storage_t::const_iterator arg_iter = args_.begin();
	const std::string& type = boost::any_cast< std::string >(*arg_iter++);
	const std::string& name = boost::any_cast< std::string >(*arg_iter++);
	assert(arg_iter == args_.end()-1);
	
#ifndef NDEBUG
	std::cout << "creating block of type: '" << type << "' with name '" << name << "'" << std::endl;
#endif
	Block *b = BlockLoader::instance().new_block(type);

	if(not b->is_configured())
	{
#ifndef NDEBUG
		std::cout << "needs additional arguments..." << std::endl;
#endif
		arg_storage_t block_param_vec = boost::any_cast< arg_storage_t >(*arg_iter);
		arg_storage_t::const_iterator block_params = block_param_vec.begin();
		arg_storage_t::const_iterator params_end =   block_param_vec.end();
#ifndef NDEBUG
		std::cout << "number of parameters given: " << block_param_vec.size() << std::endl;
#endif
		if(b->get_params().size() != block_param_vec.size())
		{
			throw ParameterCountMismatchException(b->get_name_sys());
		}

		do
		{
			assert(boost::any_cast< Variable >(*block_params) == true);
			b->set_parameter(boost::any_cast< Variable >(*block_params++));
		} while(not b->is_configured());
	}
	else
	{
#ifndef NDEBUG
		std::cout << "does not need any additional arguments." << std::endl;
#endif
	}
	Systems::instance().get_root()->add_block(b, name);
}




BlockDescribeCommand::BlockDescribeCommand(const arg_storage_t& args) : args_(args) { }

BlockDescribeCommand::result_type BlockDescribeCommand::operator()() const
{
	arg_storage_t::const_iterator arg_iter = args_.begin();
	const std::string& name = boost::any_cast< std::string >(*arg_iter++);
	assert(arg_iter == args_.end());

	std::cout << "describe block: '"<< name << "'" << std::endl;
}




ConnectCommand::ConnectCommand(const arg_storage_t& args) : args_(args) { }

ConnectCommand::result_type ConnectCommand::operator()() const
{
	arg_storage_t::const_iterator arg_iter = args_.begin();

	const std::string& source_block = boost::any_cast< std::string >(*arg_iter++);
	const std::string& source_port  = boost::any_cast< std::string >(*arg_iter++);
	const std::string& sink_block   = boost::any_cast< std::string >(*arg_iter++);
	const std::string& sink_port    = boost::any_cast< std::string >(*arg_iter++);
	assert(arg_iter == args_.end());

	Systems::instance().get_root()->connect_ports(source_block, source_port, sink_block, sink_port);
}



RunCommand::RunCommand(const arg_storage_t& args) : args_(args) { }

RunCommand::result_type RunCommand::operator()() const
{
	arg_storage_t::const_iterator arg_iter = args_.begin();
	const uint32_t times = args_.empty() ? 1 : boost::any_cast< uint32_t >(*arg_iter++);
	assert(arg_iter == args_.end());

#ifndef NDEBUG
	std::cout << "initializing..." << std::endl;
#endif
	Systems::instance().get_root()->initialize();
#ifndef NDEBUG
	std::cout << "starting system..." << std::endl;
#endif
	Systems::instance().get_root()->wakeup_sys(times);
}



VariableDeclarationCommand::VariableDeclarationCommand(const arg_storage_t& args) : args_(args) { }

VariableDeclarationCommand::result_type VariableDeclarationCommand::operator()() const
{
	arg_storage_t::const_iterator arg_iter = args_.begin();

	const std::string& var_name = boost::any_cast< std::string >(*arg_iter++);
	const Variable& var = boost::any_cast< Variable >(*arg_iter++);
	assert(arg_iter == args_.end());

	Systems::instance().get_root()->add_variable(var_name, var);
}


} // namespace commands
