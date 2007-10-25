#ifndef _ACTIONS_HPP
#define _ACTIONS_HPP

#include <vector>
#include <iostream>
#include <boost/any.hpp>
#include "block.hpp"
#include "system.hpp"
#include "block_loader.hpp"
#include "variable.hpp"



template< template< typename > class VectorT >
struct BlockDescribeAction
{
	BlockDescribeAction(const VectorT< boost::any >& args) : args_(args) { }

	void operator()(System&, const BlockLoader&) const
	{
		const std::string& name = boost::any_cast< std::string >(args_[0]);

		std::cout << "describe block: '"<< name << "'" << std::endl;
	}

	typedef void result_type;

	const VectorT< boost::any >& args_;
};



template< template< typename > class VectorT >
inline BlockDescribeAction< VectorT > block_describer(VectorT< boost::any >& v)
{
	return BlockDescribeAction< VectorT >(v);
}



struct BlockAddAction
{
	template< template< typename > class VectorT >
	void operator()(const VectorT< boost::any >& args, System & sys, const BlockLoader& bl) const
	{
		const std::string& type = boost::any_cast< std::string >(args[0]);
		const std::string& name = boost::any_cast< std::string >(args[1]);
#ifndef NDEBUG
		std::cout << "creating block of type: '" << type << "' with name '" << name << "'" << std::endl;
#endif
		Block *b = bl.new_block(type);

		if(not b->is_configured())
		{
#ifndef NDEBUG
			std::cout << "needs additional arguments..." << std::endl;
#endif
			do
			{
				std::cout << b->get_parameter_description() << std::endl;
				std::string inp;
				std::getline(std::cin, inp);

				Variable v;
				v.parse_input(inp);

				std::cout << v.get_dimensions()[0] << "x"<< v.get_dimensions()[1] <<std::endl;
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
};



struct ConnectAction
{
	template< template< typename > class VectorT >
	void operator()(const VectorT< boost::any >& args, System & sys, const BlockLoader&) const
	{
		const std::string& source_block = boost::any_cast< std::string >(args[0]);
		const std::string& source_port  = boost::any_cast< std::string >(args[1]);
		const std::string& sink_block   = boost::any_cast< std::string >(args[2]);
		const std::string& sink_port    = boost::any_cast< std::string >(args[3]);

#ifndef NDEBUG
		std::cout << "Connecting " << source_block << ":" << source_port << "->" << sink_block << ":" << sink_port << std::endl;
#endif
		sys.connect_ports(source_block, source_port, sink_block, sink_port);
	}

	typedef void result_type;
};



struct RunAction
{
	template< template< typename > class VectorT >
	void operator()(const VectorT< boost::any >& args, System & sys, const BlockLoader&) const
	{
		const uint32_t times = args.empty() ? 1u : boost::any_cast< uint32_t >(args[0]);
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
};

#endif // _ACTIONS_HPP
