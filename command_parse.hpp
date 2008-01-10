#ifndef _COMMAND_PARSE_HPP
#define _COMMAND_PARSE_HPP

#ifndef NDEBUG
#include <iostream>
#endif

#include <vector>
#include <list>
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/lambda/lambda.hpp>

#include "variable_parser.hpp"
#include "input/actions.hpp"


namespace hump
{
    static const std::string add_block_command = "add_block";
    static const std::string describe_block_command = "describe_block";
    static const std::string connect_command = "connect";
    static const std::string run_command = "run";
}



using namespace boost::spirit;
using phoenix::construct_;
using phoenix::arg1;
using phoenix::arg2;
using boost::ref;



#ifndef NDEBUG
namespace std
{
    template< typename C, typename E, typename T >
    basic_ostream< C, E > & operator<<(basic_ostream< C, E > & out, std::vector< T > const & what)
    {
        for_each
        (
            what.begin(),
            what.end(),
            out << boost::lambda::_1 << " "
        );
        return out;
    }

    template< typename C, typename E >
    basic_ostream< C, E > & operator<<(basic_ostream< C, E > & out, std::vector< boost::any > const & what)
    {
        out << "std::vector<boost::any>, length: "<< what.size();
        return out;
    }
}
#endif



struct CommandClosure : boost::spirit::closure< CommandClosure, boost::function< void(System &) > >
{
    member1 command;
};



namespace {

struct IDENTIFIER : grammar< IDENTIFIER, ResultClosure< std::string >::context_t >
{
    template< typename ScannerT >
    struct definition {
        typedef rule< ScannerT > rule_t;
        rule_t main;

        rule_t const& start() const
        {
            return main;
        }

        definition(IDENTIFIER const& self)
        {
            main = (
                lexeme_d
                [
                    (+(alnum_p | '_'))
                    [
                        self.result_ = construct_< std::string >(arg1, arg2)
                    ]
                ]
            );
            BOOST_SPIRIT_DEBUG_RULE(main);
        }
    };
} IDENTIFIER_g;



struct VARIABLE_NAME : grammar< VARIABLE_NAME, ResultClosure< std::string >::context_t >
{
    template< typename ScannerT >
    struct definition {
        typedef rule< ScannerT > rule_t;
        rule_t main;

        rule_t const& start() const
        {
            return main;
        }

        definition(VARIABLE_NAME const& self)
        {
            main =
                lexeme_d
                [
                    ch_p('$')
                    >> IDENTIFIER_g[ self.result_ = construct_< std::string >(arg1) ]
                ]
                ;
            BOOST_SPIRIT_DEBUG_RULE(main);
        }
    };
} VARIABLE_NAME_g;



template< template< typename > class VectorT >
struct PARAMETER_LIST : grammar< PARAMETER_LIST< VectorT >, typename ResultClosure< VectorT< boost::any > >::context_t >
{
    template< typename ScannerT >
    struct definition {
        typedef rule< ScannerT > rule_t;
        rule_t main;

        rule_t const& start() const
        {
            return main;
        }

        definition(PARAMETER_LIST const& self)
        {
            main
                =
                    ch_p('(')
                    >> !(
                           ( VARIABLE_NAME_g[ var_lookup_a(self.result_()) ]
                           | VARIABLE_g[ push_back_a(self.result_()) ]
                           )  % ch_p(',')
                        )
                    >> ch_p(')')
                ;
            BOOST_SPIRIT_DEBUG_RULE(main);
        }
    };
};

PARAMETER_LIST< std::vector > PARAMETER_LIST_g;
}


template< template< typename > class ArgContT >
struct CommandParser : public grammar< CommandParser< ArgContT >, CommandClosure::context_t >
{
    CommandParser(ArgContT< boost::any >& v) : args_(v) { }

    template < typename ScannerT >
    struct definition
    {
        definition(CommandParser const& self)
        {
            using namespace phoenix;
            main = (
                comm
                    =    (    eps_p[ clear_a(self.args_) ]
                              >>  block_add            [ self.command = block_add_a_interactive(self.args_) ]
                               |  block_describe       [ self.command = block_describe_a(self.args_) ]
                               |  ports_connect        [ self.command = connect_a(self.args_) ]
                               |  run                  [ self.command = run_a(self.args_) ]
                               |  variable_declaration [ self.command = var_decl_a(self.args_) ]
                         )
                         >> ch_p(';')
                    ,

                block_add
                    =    strlit< std::string::const_iterator >(hump::add_block_command.begin(), hump::add_block_command.end())
                         >> IDENTIFIER_g     [ push_back_a(self.args_) ]
                         >> IDENTIFIER_g     [ push_back_a(self.args_) ]
                         >> PARAMETER_LIST_g [ push_back_a(self.args_) ]
                    ,

                block_describe
                    =    strlit< std::string::const_iterator >(hump::describe_block_command.begin(), hump::describe_block_command.end())
                         >> IDENTIFIER_g[ push_back_a(self.args_) ]
                    ,

                ports_connect
                    =    strlit< std::string::const_iterator >(hump::connect_command.begin(), hump::connect_command.end())
                         >> IDENTIFIER_g[ push_back_a(self.args_) ]
                         >> ':'
                         >> IDENTIFIER_g[ push_back_a(self.args_) ]
                         >> "->"
                         >> IDENTIFIER_g[ push_back_a(self.args_) ]
                         >> ':'
                         >> IDENTIFIER_g[ push_back_a(self.args_) ]
                    ,

                run
                    =    strlit< std::string::const_iterator >(hump::run_command.begin(), hump::run_command.end())
                         >> !uint_p[ push_back_a(self.args_) ]
                    ,

                variable_declaration
                    =    VARIABLE_NAME_g[ push_back_a(self.args_) ]
                         >> '='
                         >> VARIABLE_g  [ push_back_a(self.args_) ]
                    );

            BOOST_SPIRIT_DEBUG_RULE(main);
            BOOST_SPIRIT_DEBUG_NODE(self);
            BOOST_SPIRIT_DEBUG_RULE(comm);
            BOOST_SPIRIT_DEBUG_NODE(block_add);
            BOOST_SPIRIT_DEBUG_RULE(block_describe);
            BOOST_SPIRIT_DEBUG_RULE(ports_connect);
            BOOST_SPIRIT_DEBUG_RULE(run);
            BOOST_SPIRIT_DEBUG_RULE(variable_declaration);
            BOOST_SPIRIT_DEBUG_NODE(IDENTIFIER_g);
            BOOST_SPIRIT_DEBUG_NODE(VARIABLE_g);
            BOOST_SPIRIT_DEBUG_NODE(VARIABLE_NAME_g);
            BOOST_SPIRIT_DEBUG_NODE(PARAMETER_LIST_g);
        }

        rule< ScannerT > main;

        subrule< 0 > comm;
        subrule< 1 > block_add;
        subrule< 2 > block_describe;
        subrule< 3 > ports_connect;
        subrule< 4 > run;
        subrule< 5 > variable_declaration;

        rule< ScannerT > const& start() const
        {
            return main;
        }
    };

    ArgContT< boost::any >& args_;
} ;

#endif // _COMMAND_PARSE_HPP
