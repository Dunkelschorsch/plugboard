#ifndef _ACTIONS_HPP
#define _ACTIONS_HPP

#include <boost/any.hpp>
#include <vector>



typedef std::vector< boost::any > arg_storage_t;


struct VariableLookupAction
{
	typedef void result_type;

	VariableLookupAction(arg_storage_t& args);
	result_type operator()(const std::string& var_name) const;

	arg_storage_t& args_;
};



inline VariableLookupAction var_lookup_a(arg_storage_t& v)
{
	return VariableLookupAction(v);
}



namespace commands
{

struct BlockAddCommand
{
	typedef void result_type;

	BlockAddCommand(const arg_storage_t& args);
	result_type operator()() const;

	const arg_storage_t& args_;
};



inline BlockAddCommand add_block(arg_storage_t& v)
{
	return BlockAddCommand(v);
}



struct BlockDescribeCommand
{
	typedef void result_type;

	BlockDescribeCommand(const arg_storage_t& args);
	result_type operator()() const;
	
	const arg_storage_t& args_;
};



inline BlockDescribeCommand describe_block(arg_storage_t& v)
{
	return BlockDescribeCommand(v);
}



struct ConnectCommand
{
	typedef void result_type;

	ConnectCommand(const arg_storage_t& args);
	result_type operator()() const;

	const arg_storage_t& args_;
};



inline ConnectCommand connect(arg_storage_t& v)
{
	return ConnectCommand(v);
}



struct RunCommand
{
	typedef void result_type;

	RunCommand(const arg_storage_t& args);
	result_type operator()() const;

	const arg_storage_t& args_;
};



inline RunCommand run(arg_storage_t& v)
{
	return RunCommand(v);
}



struct VariableDeclarationCommand
{
	typedef void result_type;

	VariableDeclarationCommand(const arg_storage_t& args);
	result_type operator()() const;

	const arg_storage_t& args_;
};



inline VariableDeclarationCommand declare_variable(arg_storage_t& v)
{
	return VariableDeclarationCommand(v);
}

} // namespace commands
#endif // _ACTIONS_HPP
