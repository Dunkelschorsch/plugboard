#ifndef _ACTIONS_HPP
#define _ACTIONS_HPP

#include <vector>
#include <iostream>
#include <boost/any.hpp>
#include "block.hpp"
#include "system.hpp"
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
	BlockDescribeAction(const VectorT< boost::any >& args) : args_(args.begin(), args.end()) { }

	template< typename SystemT >
	void operator()(SystemT&) const
	{
		const std::string& name = boost::any_cast< std::string >(args_[0]);

		std::cout << "describe block: '"<< name << "'" << std::endl;
	}

	typedef void result_type;

	const VectorT< boost::any > args_;
};



template< template< typename > class VectorT >
inline BlockDescribeAction< VectorT > block_describe_a(VectorT< boost::any >& v)
{
	return BlockDescribeAction< VectorT >(v);
}



template< template< typename > class VectorT >
struct BlockAddAction_interactive
{
	BlockAddAction_interactive(const VectorT< boost::any >& args) : args_(args.begin(), args.end()) { }

	template< typename SystemT >
	void operator()(SystemT & sys) const
	{
		const std::string& type = boost::any_cast< std::string >(args_[0]);
		const std::string& name = boost::any_cast< std::string >(args_[1]);
#ifndef NDEBUG
		std::cout << "creating block of type: '" << type << "' with name '" << name << "'" << std::endl;
#endif
		Block *b = BlockLoader::instance().new_block(type);

		if(not b->is_configured())
		{
			std::cout << "needs additional arguments..." << std::endl;

			do
			{
				std::cout << b->get_parameter_description() << ": ";
				std::string inp;
				std::getline(std::cin, inp);

				Variable v;
				v.parse_input(inp);

				b->set_parameter(v);
			} while(not b->is_configured());
		}
		else
		{
#ifndef NDEBUG
			std::cout << "does not need any additional arguments." << std::endl;
#endif
		}
		sys.add_block(b, name);
	}

	typedef void result_type;

	VectorT< boost::any > args_;
};



template< template< typename > class VectorT >
inline BlockAddAction_interactive< VectorT > block_add_a_interactive(VectorT< boost::any >& v)
{
	return BlockAddAction_interactive< VectorT >(v);
}



template< template< typename > class VectorT >
struct ConnectAction
{
	ConnectAction(const VectorT< boost::any >& args) : args_(args.begin(), args.end()) { }

	template< typename SystemT >
	void operator()(SystemT & sys) const
	{
		const std::string& source_block = boost::any_cast< std::string >(args_[0]);
		const std::string& source_port  = boost::any_cast< std::string >(args_[1]);
		const std::string& sink_block   = boost::any_cast< std::string >(args_[2]);
		const std::string& sink_port    = boost::any_cast< std::string >(args_[3]);

		sys.connect_ports(source_block, source_port, sink_block, sink_port);
	}

	typedef void result_type;

	const VectorT< boost::any > args_;
};



template< template< typename > class VectorT >
inline ConnectAction< VectorT > connect_a(VectorT< boost::any >& v)
{
	return ConnectAction< VectorT >(v);
}



template< template< typename > class VectorT >
struct RunAction
{
	RunAction(const VectorT< boost::any >& args) : args_(args.begin(), args.end()) { }

	template< typename SystemT >
	void operator()(SystemT & sys) const
	{
		const uint32_t times = args_.empty() ? 1 : boost::any_cast< uint32_t >(args_[0]);
#ifndef NDEBUG
		std::cout << "initializing..." << std::endl;
#endif
		sys.initialize();
#ifndef NDEBUG
		std::cout << "starting system..." << std::endl;
#endif
		sys.wakeup_sys(times);
	}

	typedef void result_type;

	const VectorT< boost::any > args_;
};



template< template< typename > class VectorT >
inline RunAction< VectorT > run_a(VectorT< boost::any >& v)
{
	return RunAction< VectorT >(v);
}

#endif // _ACTIONS_HPP
