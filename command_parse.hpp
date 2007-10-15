#ifndef _COMMAND_PARSE_HPP
#define _COMMAND_PARSE_HPP

#include <vector>
#include <iostream>

#include <boost/any.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
// #include <boost/spirit/actor/assign_actor.hpp>

#include "system.hpp"
#include "block_loader.hpp"



namespace hump
{
	static const std::string add_block_command = "add block";
	static const std::string describe_block_command = "describe block";
	static const std::string connect_command = "connect";
	static const std::string run_command = "run";
}



using namespace boost::spirit;
using boost::ref;



namespace std
{
	template< typename C, typename E, typename T >
	basic_ostream<C, E> & operator<<(basic_ostream<C, E> & out, std::vector< T > const & what)
	{
		return out << "vector output not yet implemented " << std::endl;
	}
}



struct BlockDescribeExecutor
{
	void operator()(char const* blockname, char const*) const
	{
		std::cout << "describe block: '"<< blockname << "'" << std::endl;
	}
};



struct BlockAddAction
{
	template< typename T0 >
	void operator()(const T0 args, System & sys, const BlockLoader & bl) const
	{
		std::string type = boost::any_cast< typeof(type) >(args[0]);
		std::string name = boost::any_cast< typeof(name) >(args[1]);
#ifndef NDEBUG
		std::cout << "creating block of type : '" << type << "' with name '" << name << "'" << std::endl;
#endif
		Block *b = bl.new_block(type);

		sys.add_block(b, name);
	}
};



struct ConnectAction
{
	template< typename T0 >
	void operator()(const T0 args, System & sys, const BlockLoader&) const
	{
		const std::string& source_block = boost::any_cast< typeof(source_block) >(args[0]);
		const std::string& source_port  = boost::any_cast< typeof(source_port) > (args[1]);
		const std::string& sink_block   = boost::any_cast< typeof(sink_block) >  (args[2]);
		const std::string& sink_port    = boost::any_cast< typeof(sink_port) >   (args[3]);
#ifndef NDEBUG
		std::cout << "Connecting " << source_block << ":" << source_port << "->" << sink_block << ":" << sink_port << std::endl;
#endif
		sys.connect_ports(source_block, source_port, sink_block, sink_port);
	}
};



struct RunAction
{
	template< typename T0 >
	void operator()(const T0 args, System & sys, const BlockLoader&) const
	{
		const uint32_t times = args.empty() ? 1u : boost::any_cast< typeof(times) >(args[0]);
#ifndef NDEBUG
		std::cout << "initializing..." << std::endl;
#endif
		sys.initialize();
#ifndef NDEBUG
		std::cout << "starting system..." << std::endl;
#endif
		sys.wakeup_sys(times);
	}
};



struct CommandClosure : boost::spirit::closure< CommandClosure, boost::function< void(System &, BlockLoader &) > >
{
	member1 command;
};



struct NameClosure : boost::spirit::closure< NameClosure, std::string >
{
	member1 name;
};



struct CommandParser : public grammar< CommandParser, CommandClosure::context_t >
{
	CommandParser(std::vector< boost::any >& v) : args_(v) {} ;

	template < typename ScannerT >
	struct definition
	{
		definition(CommandParser const& self)
		{
			using namespace phoenix;
			first =
			    (
				  eps_p[boost::bind(&std::vector<boost::any>::clear, ref(self.args_))]
			       >> block_add
				  [
					self.command=boost::bind< void >(BlockAddAction(), ref(self.args_), _1, _2)
				  ]
				| block_describe[BlockDescribeExecutor()]
				  [
					BlockDescribeExecutor()
				  ]

				| ports_connect
				  [
					self.command=boost::bind< void >(ConnectAction(), ref(self.args_), _1, _2)
				  ]
				| run
				  [
					self.command=boost::bind< void >(RunAction(), ref(self.args_), _1, _2)
				  ]
			    ) >> ch_p(';') 
			    ;

			block_add =
			    strlit< std::string::const_iterator >(hump::add_block_command.begin(), hump::add_block_command.end())
			    >>
			    lexeme_d
			    [
				identifier[push_back_a(self.args_)]
			    ]
			    >>
			    lexeme_d
			    [
				identifier[push_back_a(self.args_)]
			    ]
			    ;

			block_describe =
			    strlit< std::string::const_iterator >(hump::describe_block_command.begin(), hump::describe_block_command.end())
			    >> lexeme_d[identifier]
			    ;

			ports_connect =
			    strlit< std::string::const_iterator >(hump::connect_command.begin(), hump::connect_command.end())
			    >> lexeme_d[identifier[push_back_a(self.args_)]]
			    >> ':'
			    >> lexeme_d[identifier[push_back_a(self.args_)]]
			    >> "->"
			    >> lexeme_d[identifier[push_back_a(self.args_)]]
			    >> ':'
			    >> lexeme_d[identifier[push_back_a(self.args_)]]
			    ;

			run =
			    strlit< std::string::const_iterator >(hump::run_command.begin(), hump::run_command.end())
			    >> !uint_p[push_back_a(self.args_)]
			    ;

			identifier =
			        (+(alnum_p | '_'))[identifier.name=construct_<std::string>(arg1, arg2)]
			    ;

 			BOOST_SPIRIT_DEBUG_RULE(self);
			BOOST_SPIRIT_DEBUG_RULE(first);
			BOOST_SPIRIT_DEBUG_RULE(block_add);
			BOOST_SPIRIT_DEBUG_RULE(block_describe);
			BOOST_SPIRIT_DEBUG_RULE(ports_connect);
			BOOST_SPIRIT_DEBUG_RULE(run);
			BOOST_SPIRIT_DEBUG_RULE(identifier);
		}

		rule<typename lexeme_scanner< ScannerT >::type, NameClosure::context_t > identifier;
		rule< ScannerT > first, block_describe, block_add, ports_connect, run;

		rule< ScannerT > const& start() const
		{
			return first;
		}
	};

	std::vector< boost::any >& args_;
};

#endif // _COMMAND_PARSE_HPP
