#ifndef _COMMAND_PARSE_HPP
#define _COMMAND_PARSE_HPP

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>

#include "system.hpp"
#include "block_loader.hpp"
#include <vector>
#include <iostream>

#include <utility>

namespace hump
{
	static const std::string add_block_command = "add block";
	static const std::string describe_block_command = "describe block";
	static const std::string connect_command = "connect";
}


using namespace boost::spirit;
using boost::ref;


namespace std
{
	template< typename C, typename E, typename T >
	basic_ostream<C, E> & operator<<(basic_ostream<C, E> & out, std::vector< T > const & what)
	{
		return out << "( sorry!! )";
	}
}



struct BlockDescribeExecutor
{
	void operator()(char const* blockname, char const*) const
	{
		std::cout << "describe block: '"<< blockname << "'" << std::endl;
	}
};



struct BlockAddClosure : boost::spirit::closure< BlockAddClosure, std::string, std::string >
{
	member1 block_name;
	member2 name_sys;
};



struct BlockAddExecutor
{
	template< typename T0, typename T1 >
	void operator()(const T0 block_type, const T1 s2, System * sys, BlockLoader * bl) const
	{
		std::cout << "creating block of type : '" << block_type() << "' with name '" << s2() << "'" << std::endl;
		Block *b = bl->new_block(block_type());
	}
};



struct ConnectClosure : boost::spirit::closure< ConnectClosure, std::vector< std::string > >
{
	member1 stuff;
};



struct ConnectExecutor
{
	template< typename IteratorT >
	void operator()(IteratorT first, IteratorT last) const
	{
		std::cout << std::string(first, last) << std::endl;
	}
};



struct CommandParser : public grammar< CommandParser >
{
	CommandParser(System * sys, BlockLoader * bl) : sys_(sys), bl_(bl) { };

	template < typename ScannerT >
	struct definition
	{
		definition(CommandParser const& self)
		{
			BlockAddExecutor f;

			using namespace phoenix;
			first =
			    (
				block_add
			    	| block_describe[BlockDescribeExecutor()]
			    	| ports_connect//[ConnectExecutor()]
			    ) >> ';'
			    ;

			block_add =
			    strlit< std::string::const_iterator >(hump::add_block_command.begin(), hump::add_block_command.end())
			    >> block_add_args[
				boost::bind<void>(f, block_add.block_name, block_add.name_sys, self.sys_, self.bl_)
			    ]
			   
			    ;

			block_describe =
			    strlit< std::string::const_iterator >(hump::describe_block_command.begin(), hump::describe_block_command.end())
			    >> identifier
			    ;

			ports_connect =
			    strlit< std::string::const_iterator >(hump::connect_command.begin(), hump::connect_command.end())
			    >> identifier
			    >> str_p(":")
			    >> identifier
			    >> str_p("->")
			    >> identifier
			    >> str_p(":")
			    >> identifier
			    ;

			identifier =
			    	+(alnum_p | '_') - ';'
			    ;

			block_name =
			        +(alnum_p | '_') - ';'
			    ;

			block_add_args =
			    lexeme_d
			    [
				block_name[block_add.block_name=construct_<std::string>(arg1, arg2)]
			    ] >>
			    lexeme_d
			    [
				block_name[block_add.name_sys=construct_<std::string>(arg1, arg2)]
			    ]
			    ;

			BOOST_SPIRIT_DEBUG_RULE(first);
			BOOST_SPIRIT_DEBUG_RULE(block_add);
			BOOST_SPIRIT_DEBUG_RULE(block_describe);
			BOOST_SPIRIT_DEBUG_RULE(ports_connect);
			BOOST_SPIRIT_DEBUG_RULE(identifier);
			BOOST_SPIRIT_DEBUG_RULE(block_add_args);
			BOOST_SPIRIT_DEBUG_RULE(block_name);
		}

		rule< ScannerT > first, block_describe;
		rule< ScannerT > identifier, block_add_args;

		rule< ScannerT, BlockAddClosure::context_t > block_add;
		rule< ScannerT, ConnectClosure::context_t > ports_connect;

		rule<typename lexeme_scanner< ScannerT >::type> block_name;

		rule< ScannerT > const& start() const
		{
			return first;
		}
	};

	System * sys_;
	BlockLoader * bl_;
};

#endif // _COMMAND_PARSE_HPP
