#ifndef _ACTIONS_HPP
#define _ACTIONS_HPP

#include <cassert>
#include <vector>
#include <iostream>
#include <boost/any.hpp>
#include "block.hpp"
#include "system.hpp"
#include "systems.hpp"
#include "block_loader.hpp"
#include "variable.hpp"


template< typename ContainerT >
struct ClearAction
{
	ClearAction(ContainerT& args) : args_(args) { }

	template< typename T0 >
	void operator()(T0) const
	{
		args_.clear();
	}

	template< typename T0, typename T1 >
	void operator()(T0, T1) const
	{
		args_.clear();
	}

	typedef void result_type;

	ContainerT& args_;
};



template< typename ContainerT >
inline ClearAction< ContainerT > clear_a(ContainerT& v)
{
	return ClearAction< ContainerT >(v);
}



template< template< typename > class VectorT >
struct BlockDescribeAction
{
	BlockDescribeAction(const VectorT< boost::any >& args) : args_(args) { }

	template< typename SystemT >
	void operator()(SystemT&) const
	{
		const std::string& name = boost::any_cast< std::string >(args_[0]);

		std::cout << "describe block: '"<< name << "'" << std::endl;
	}

	typedef void result_type;

	const VectorT< boost::any >& args_;
};



template< template< typename > class VectorT >
inline BlockDescribeAction< VectorT > block_describe_a(VectorT< boost::any >& v)
{
	return BlockDescribeAction< VectorT >(v);
}



template< template< typename > class VectorT >
struct BlockAddAction
{
	BlockAddAction(const VectorT< boost::any >& args) : args_(args) { }

	template< typename SystemT >
	void operator()(SystemT &) const
	{
		typename VectorT< boost::any >::const_iterator arg_iter = args_.begin();
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
			VectorT< boost::any > block_param_vec = boost::any_cast< VectorT< boost::any > >(*arg_iter);
			typename VectorT< boost::any >::const_iterator block_params = block_param_vec.begin();
			typename VectorT< boost::any >::const_iterator params_end =   block_param_vec.end();
#ifndef NDEBUG
			std::cout << "number of parameters given: " << block_param_vec.size() << std::endl;
#endif
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

	typedef void result_type;

	const VectorT< boost::any >& args_;
};



template< template< typename > class VectorT >
inline BlockAddAction< VectorT > block_add_a_interactive(VectorT< boost::any >& v)
{
	return BlockAddAction< VectorT >(v);
}



template< class VectorT >
struct ConnectAction
{
	ConnectAction(const VectorT& args) : args_(args) { }

	template< typename SystemT >
	void operator()(SystemT &) const
	{
		typename VectorT::const_iterator arg_iter = args_.begin();

		const std::string& source_block = boost::any_cast< std::string >(*arg_iter++);
		const std::string& source_port  = boost::any_cast< std::string >(*arg_iter++);
		const std::string& sink_block   = boost::any_cast< std::string >(*arg_iter++);
		const std::string& sink_port    = boost::any_cast< std::string >(*arg_iter++);
		assert(arg_iter == args_.end());
	
		Systems::instance().get_root()->connect_ports(source_block, source_port, sink_block, sink_port);
	}

	typedef void result_type;

	const VectorT& args_;
};



template< class VectorT >
inline ConnectAction< VectorT > connect_a(VectorT& v)
{
	return ConnectAction< VectorT >(v);
}



template< class VectorT >
struct VariableDeclarationAction
{
	VariableDeclarationAction(const VectorT& args) : args_(args) { }

	template< typename SystemT >
	void operator()(SystemT &) const
	{
		typename VectorT::const_iterator arg_iter = args_.begin();

		const std::string& var_name = boost::any_cast< std::string >(*arg_iter++);
		const Variable& var = boost::any_cast< Variable >(*arg_iter++);
		assert(arg_iter == args_.end());

		Systems::instance().get_root()->add_variable(var_name, var);
	}

	typedef void result_type;

	const VectorT& args_;
};



template< class VectorT >
inline VariableDeclarationAction< VectorT > var_decl_a(VectorT& v)
{
	return VariableDeclarationAction< VectorT >(v);
}



template< class VectorT >
struct VariableLookupAction
{
	VariableLookupAction(VectorT& args) : args_(args) { }

	void operator()(const std::string& var_name) const
	{
		typename VectorT::const_iterator arg_iter = args_.begin();
#ifndef NDEBUG
		std::cout << "Variable to look up: '" << var_name << "'." << std::endl;
#endif
		const Variable& vv = Systems::instance().get_root()->get_variable(var_name);
		assert(vv == true);
		args_.push_back(vv);
	}

	typedef void result_type;

	VectorT& args_;
};



template< class VectorT >
inline VariableLookupAction< VectorT > var_lookup_a(VectorT& v)
{
	return VariableLookupAction< VectorT >(v);
}



template< template< typename > class VectorT >
struct RunAction
{
	RunAction(const VectorT< boost::any >& args) : args_(args) { }

	template< typename SystemT >
	void operator()(SystemT &) const
	{
		const uint32_t times = args_.empty() ? 1 : boost::any_cast< uint32_t >(args_[0]);
#ifndef NDEBUG
		std::cout << "initializing..." << std::endl;
#endif
		Systems::instance().get_root()->initialize();
#ifndef NDEBUG
		std::cout << "starting system..." << std::endl;
#endif
		Systems::instance().get_root()->wakeup_sys(times);
	}

	typedef void result_type;

	const VectorT< boost::any >& args_;
};



template< template< typename > class VectorT >
inline RunAction< VectorT > run_a(VectorT< boost::any >& v)
{
	return RunAction< VectorT >(v);
}

#endif // _ACTIONS_HPP
