/*!
 * \file
 * \brief
 * \author Armin Schmidt
 *
 * ----------------------------------------------------------------------------
 *
 * PlugBoard - A versatile communication simulation framework
 * Copyright (C) 2007-2008  Armin Schmidt
 *
 * This file is part of PlugBoard.
 *
 * PlugBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PlugBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PlugBoard.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ----------------------------------------------------------------------------
 */

#ifndef NEW_PARSE_HPP
#define NEW_PARSE_HPP

#include <boost/spirit/core.hpp>
#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/error_handling/exceptions.hpp>

#include "grammar/command/parser_id.hpp"
namespace plugboard
{

enum parse_errors
{
    opening_paren_expected,
    closing_paren_expected
};

using namespace boost::spirit;

struct command_parser : public grammar< command_parser >
{
    template <typename ScannerT>
    struct definition
    {
        definition(command_parser const& /*self*/)
        {
            assertion< parse_errors > expect_opening_paren(opening_paren_expected);
            assertion< parse_errors > expect_closing_paren(closing_paren_expected);

            paren_open
                =    expect_opening_paren( ch_p('(') )
                ;

            paren_close
                =    expect_closing_paren( ch_p(')') )
                ;

            first
                =    ( assignment | command ) >> no_node_d[ ch_p(';') ]
                ;

            command
                =      add_block_command
                     | connect_command
                     | run_command
                ;

            add_block_command
                =       no_node_d[ str_p("add_block") ]
                     >> lexeme_d[ identifier ]  // block type
                     >> lexeme_d[ identifier ]  // block name
                     >> inner_node_d
                        [
                               paren_open
                            >> !infix_node_d
                               [
                                   ( string_lit | scalar_expression ) % ','  // block parameter list
                               ]
                            >> paren_close
                        ]
                ;

            connect_command
                =       no_node_d[ str_p("connect") ]
                     >> infix_node_d
                        [
                               lexeme_d[ identifier ]  // source block name
                            >> ':'
                            >> lexeme_d[ identifier ]  // source port name
                            >> "->"
                            >> lexeme_d[ identifier ]  // sink block name
                            >> ':'
                            >> lexeme_d[ identifier ]  // sink port name
                        ]
                ;

            run_command
                =    str_p("run") >> !scalar_expression
                ;

            array
                =     n_range
                    | no_node_d[ ch_p('[') ]
                           >> ( row_vec % no_node_d[ ch_p(';') ] ) >>
                      no_node_d[ ch_p(']') ]
                ;

            row_vec
                =    scalar_expression % no_node_d[ !ch_p(',') ]
                ;

            string_lit
                =    inner_node_d
                     [
                         '"'
                         >> leaf_node_d[ lexeme_d[ +( print_p - '"' ) ] ] >>
                         '"'
                     ]
                ;

            primitive
                =      realnum
                     | integernum
                     | complex_num
                     | variable
                ;

            realnum
                =    leaf_node_d[ strict_real_p ]
                ;

            complex_num
                =    leaf_node_d[ ch_p('i') | ch_p('j') ]
                ;

            integernum
                =    leaf_node_d[ int_p ]
                ;

            variable
                =    lexeme_d[ no_node_d[ ch_p('$') ] >> identifier ]
                ;

            n_range
                =    infix_node_d
                     [
                            scalar_expression  // start value
                         >> ':'
                         >> scalar_expression  // increment
                         >> ':'
                         >> scalar_expression  // end value
                     ]
                ;

            assignment
                =      variable  // variable to assign value
                    >> no_node_d[ ch_p('=') ]
                    >> ( string_lit
                       | array
                       | scalar_expression
                       )
                ;

            scalar_factor
                =      primitive
                     | inner_node_d[ '(' >> scalar_expression >> ')' ]
                     | ( root_node_d[ ch_p('-') ] >> scalar_factor )
                ;

            scalar_pot // this one kinda doesn't work
                =       scalar_factor
                     >> *( root_node_d[ ch_p('^') ] >> scalar_factor )
                ;

            scalar_term
                =       root_node_d[ scalar_pot ]
                     >> *( ( root_node_d[ ch_p('*') ] >> scalar_pot )
                         | ( root_node_d[ ch_p('/') ] >> scalar_pot )
                         )
                ;

            scalar_expression
                =       scalar_term
                     >> *( ( root_node_d[ ch_p('+') ] >> scalar_term )
                         | ( root_node_d[ ch_p('-') ] >> scalar_term )
                         )
                ;

            identifier
                =    leaf_node_d[ +( alnum_p | '_' ) ]
                ;
        }

        rule< ScannerT, parser_context<>, parser_tag< parserID::integer > > integernum;
        rule< ScannerT, parser_context<>, parser_tag< parserID::real > > realnum;
        rule< ScannerT, parser_context<>, parser_tag< parserID::complex > > complex_num;
        rule< ScannerT, parser_context<>, parser_tag< parserID::primitive > > primitive;
        rule< ScannerT, parser_context<>, parser_tag< parserID::string > > string_lit;
        rule< ScannerT, parser_context<>, parser_tag< parserID::range > > n_range;
        rule< ScannerT, parser_context<>, parser_tag< parserID::array > > array;
        rule< ScannerT, parser_context<>, parser_tag< parserID::rowvec > > row_vec;

        rule< ScannerT, parser_context<>, parser_tag< parserID::variable > > variable;

        rule< ScannerT, parser_context<>, parser_tag< parserID::scalarfactor > > scalar_factor;
        rule< ScannerT, parser_context<>, parser_tag< parserID::scalarterm > > scalar_term;
        rule< ScannerT, parser_context<>, parser_tag< parserID::scalarexpression > > scalar_expression;
        rule< ScannerT, parser_context<>, parser_tag< parserID::scalarpot > > scalar_pot;
        rule< ScannerT, parser_context<>, parser_tag< parserID::assignment > > assignment;

        rule< ScannerT, parser_context<>, parser_tag< parserID::command > > command;
        rule< ScannerT, parser_context<>, parser_tag< parserID::addblock > > add_block_command;
        rule< ScannerT, parser_context<>, parser_tag< parserID::connect > > connect_command;
        rule< ScannerT, parser_context<>, parser_tag< parserID::run > > run_command;

        rule< typename lexeme_scanner< ScannerT >::type, parser_context<>, parser_tag< parserID::identifier > > identifier;

        rule< ScannerT, parser_context<>, parser_tag< parserID::openparen > > paren_open;
        rule< ScannerT, parser_context<>, parser_tag< parserID::openparen > > paren_close;

        rule< ScannerT, parser_context<>, parser_tag< parserID::first > > first;

        rule<ScannerT, parser_context<>, parser_tag< parserID::first > > const& start() const
        {
            return first;
        }
    }; // struct definition
}; // struct command_parser
} // namespace plugboard

#endif
