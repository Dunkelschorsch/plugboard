#ifndef _COMMAND_PARSE_HPP
#define _COMMAND_PARSE_HPP

#include <vector>
#include <iostream>

#include <boost/any.hpp>

#include <boost/function.hpp>

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>

#include <boost/lambda/lambda.hpp>

#include "input/actions.hpp"


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
}



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
    CommandParser(std::vector< boost::any >& v) : args_(v) { }

    template < typename ScannerT >
    struct definition
    {
        definition(CommandParser const& self)
        {
            using namespace phoenix;
            comm = (
                first
                    =    (    eps_p
                              [
                                  clear_a(self.args_)
                              ]
                              >> block_add
                                 [
                                     self.command=block_add_a(self.args_)
                                 ]
                              |  block_describe
                                 [
                                     self.command=block_describe_a(self.args_)
                                 ]
                              |  ports_connect
                                 [
                                     self.command=connect_a(self.args_)
                                 ]
                              |  run
                                 [
                                     self.command=run_a(self.args_)
                                 ]
                         )
                         >> ch_p(';')
                    ,

                block_add
                    =    strlit< std::string::const_iterator >(hump::add_block_command.begin(), hump::add_block_command.end())
                         >> lexeme_d
                            [
                                identifier
                                [
                                    push_back_a(self.args_)
                                ]
                            ]
                         >> lexeme_d
                            [
                                identifier
                                [
                                    push_back_a(self.args_)
                                ]
                            ]
                    ,

                block_describe
                    =    strlit< std::string::const_iterator >(hump::describe_block_command.begin(), hump::describe_block_command.end())
                         >> lexeme_d
                            [
                                identifier
                                [
                                    push_back_a(self.args_)
                                ]
                            ]
                    ,

                ports_connect
                    =    strlit< std::string::const_iterator >(hump::connect_command.begin(), hump::connect_command.end())
                         >> lexeme_d
                            [
                                identifier
                                [
                                    push_back_a(self.args_)
                                ]
                            ]
                         >> ':'
                         >> lexeme_d
                            [
                                identifier
                                [
                                    push_back_a(self.args_)
                                ]
                            ]
                         >> "->"
                         >> lexeme_d
                            [
                                identifier
                                [
                                    push_back_a(self.args_)
                                ]
                            ]
                         >> ':'
                         >> lexeme_d
                            [
                                identifier
                                [
                                    push_back_a(self.args_)
                                ]
                            ]
                    ,

                run
                    =    strlit< std::string::const_iterator >(hump::run_command.begin(), hump::run_command.end())
                         >> !uint_p
                             [
                                 push_back_a(self.args_)
                             ]
                    ,

                identifier
                    =    (
                             +(alnum_p | '_')
                         )
                         [
                             identifier.name=construct_<std::string>(arg1, arg2)
                         ]
                   );

            BOOST_SPIRIT_DEBUG_RULE(comm);
            BOOST_SPIRIT_DEBUG_RULE(self);
            BOOST_SPIRIT_DEBUG_RULE(first);
            BOOST_SPIRIT_DEBUG_RULE(block_add);
            BOOST_SPIRIT_DEBUG_RULE(block_describe);
            BOOST_SPIRIT_DEBUG_RULE(ports_connect);
            BOOST_SPIRIT_DEBUG_RULE(run);
            BOOST_SPIRIT_DEBUG_RULE(identifier);
        }

        rule< ScannerT > comm;

        subrule< 0 > first;
        subrule< 1 > block_add;
        subrule< 2 > block_describe;
        subrule< 3 > ports_connect;
        subrule< 4 > run;
        subrule< 5, NameClosure::context_t > identifier;

        rule< ScannerT > const& start() const
        {
            return comm;
        }
    };

    std::vector< boost::any >& args_;
};

#endif // _COMMAND_PARSE_HPP
