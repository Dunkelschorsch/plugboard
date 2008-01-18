#ifndef COMMAND_PARSE_HPP
#define COMMAND_PARSE_HPP

#ifndef NDEBUG
#include <iostream>
#endif

#include <boost/function.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>

#include "grammar/variable/variable_parser.hpp"
#include "grammar/command/identifier.hpp"
#include "grammar/command/variable_name.hpp"
#include "grammar/command/parameter_list.hpp"
#include "grammar/command/actions.hpp"



using namespace boost::spirit;
using boost::ref;



namespace hump
{
    static const std::string add_block_command = "add_block";
    static const std::string describe_block_command = "describe_block";
    static const std::string connect_command = "connect";
    static const std::string run_command = "run";
}



#ifndef NDEBUG
namespace std
{
    template< typename C, typename E >
    basic_ostream< C, E > & operator<<(basic_ostream< C, E > & out, std::vector< boost::any > const & what)
    {
        out << "std::vector<boost::any>, length: "<< what.size();
        return out;
    }
}
#endif



struct CommandClosure : boost::spirit::closure< CommandClosure, boost::function< void() > >
{
    member1 command;
};



struct CommandParser : public grammar< CommandParser, CommandClosure::context_t >
{
    CommandParser(arg_storage_t& v) : args_(v) { }

    template < typename ScannerT >
    struct definition
    {
        definition(CommandParser const& self)
        {
            using namespace phoenix;
            main = (
                submain
                    =    (    eps_p[ clear_a(self.args_) ] >> !comment
                              >>  block_add            [ self.command = commands::add_block(self.args_) ]
                               |  block_describe       [ self.command = commands::describe_block(self.args_) ]
                               |  ports_connect        [ self.command = commands::connect(self.args_) ]
                               |  run                  [ self.command = commands::run(self.args_) ]
                               |  variable_declaration [ self.command = commands::declare_variable(self.args_) ]
                         )
                         >> ch_p(';') >> !comment
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
                    ,

                skip_until_eol = *(('\\' >> eol_p) | (anychar_p - eol_p))
                    ,

                comment = '#' >> skip_until_eol
                    );

            BOOST_SPIRIT_DEBUG_RULE(main);
            BOOST_SPIRIT_DEBUG_NODE(self);
            BOOST_SPIRIT_DEBUG_RULE(submain);
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

        subrule< 0 > submain;
        subrule< 1 > block_add;
        subrule< 2 > block_describe;
        subrule< 3 > ports_connect;
        subrule< 4 > run;
        subrule< 5 > variable_declaration;
        subrule< 6 > skip_until_eol;
        subrule< 7 > comment;

        rule< ScannerT > const& start() const
        {
            return main;
        }
    };

    arg_storage_t& args_;
} ;

#endif // COMMAND_PARSE_HPP
