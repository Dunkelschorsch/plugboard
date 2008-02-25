#ifndef NEW_PARSE_HPP
#define NEW_PARSE_HPP

#include <boost/spirit/core.hpp>
#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/error_handling/exceptions.hpp>

#include <iostream>
#include <string>
#include <set>

#ifndef NDEBUG
#include <boost/spirit/tree/tree_to_xml.hpp>
#include <map>
#endif

#include "types/base.hpp"
#include "variable/variable.hpp"
#include "variable/variable_iterator.hpp"
#include "block/block.hpp"
#include "block_loader.hpp"
#include "systems.hpp"


using namespace boost::spirit;


// how the input data gets to the scanner
typedef file_iterator< char > file_iter_t;
typedef position_iterator2< file_iter_t > iterator_t;
// what kind of scanning is done with the input data
typedef tree_match< iterator_t > parse_tree_match_t;
typedef ast_match_policy< iterator_t > match_policy_t;
// activate whitespace skpping
typedef skip_parser_iteration_policy< space_parser > skip_parser_policy_t;

typedef scanner_policies< skip_parser_policy_t, match_policy_t > scanner_policy_t;
typedef scanner< iterator_t, scanner_policy_t > ast_scanner_t;
typedef parse_tree_match_t::const_tree_iterator tree_iter_t;


enum parse_errors
{
    opening_paren_expected,
    closing_paren_expected
};


struct new_command_parser : public grammar< new_command_parser >
{
    static const int integerID = 1;
    static const int realID = 2;
    static const int complexID = 3;
    static const int stringID = 4;
    static const int primitiveID = 5;

    static const int rangeID = 6;
    static const int arrayID = 7;
    static const int rowvecID = 8;

    static const int variableID = 9;

    static const int identifierID = 10;

    static const int scalarfactorID = 11;
    static const int scalartermID = 12;
    static const int scalarexpressionID = 13;
    static const int scalarpotID = 14;

    static const int assignmentID = 50;

    // commands
    static const int commandID = 1000;
    static const int addblockID = 1001;
    static const int connectID = 1002;
    static const int runID = 1003;

    // start rule
    static const int firstID = 10000;

    static const int openparenID = 2001;
    static const int closeparenID = 2002;

    template <typename ScannerT>
    struct definition
    {
        definition(new_command_parser const& /*self*/)
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

//    arg_list    =   !infix_node_d[ ( string_lit | scalar_expression ) % ch_p(',') ];

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

    rule< ScannerT, parser_context<>, parser_tag< integerID > > integernum;
    rule< ScannerT, parser_context<>, parser_tag< realID > > realnum;
    rule< ScannerT, parser_context<>, parser_tag< complexID > > complex_num;
    rule< ScannerT, parser_context<>, parser_tag< primitiveID > > primitive;
    rule< ScannerT, parser_context<>, parser_tag< stringID > > string_lit;
    rule< ScannerT, parser_context<>, parser_tag< rangeID > > n_range;
    rule< ScannerT, parser_context<>, parser_tag< arrayID > > array;
    rule< ScannerT, parser_context<>, parser_tag< rowvecID > > row_vec;

    rule< ScannerT, parser_context<>, parser_tag< variableID > > variable;

    rule< ScannerT, parser_context<>, parser_tag< scalarfactorID > > scalar_factor;
    rule< ScannerT, parser_context<>, parser_tag< scalartermID > > scalar_term;
    rule< ScannerT, parser_context<>, parser_tag< scalarexpressionID > > scalar_expression;
    rule< ScannerT, parser_context<>, parser_tag< scalarpotID > > scalar_pot;
    rule< ScannerT, parser_context<>, parser_tag< assignmentID > > assignment;

    rule< ScannerT, parser_context<>, parser_tag< commandID > > command;
    rule< ScannerT, parser_context<>, parser_tag< addblockID > > add_block_command;
    rule< ScannerT, parser_context<>, parser_tag< connectID > > connect_command;
    rule< ScannerT, parser_context<>, parser_tag< runID > > run_command;

    rule< typename lexeme_scanner< ScannerT >::type, parser_context<>, parser_tag< identifierID > > identifier;

    rule< ScannerT, parser_context<>, parser_tag< openparenID > > paren_open;
    rule< ScannerT, parser_context<>, parser_tag< openparenID > > paren_close;


    rule< ScannerT, parser_context<>, parser_tag< firstID > > first;

    rule<ScannerT, parser_context<>, parser_tag<firstID> > const&
    start() const { return first; }
    };
};


std::string make_string(tree_iter_t const& i)
{
	assert(i->children.size() == 0);
	return std::string(i->value.begin(), i->value.end());
}


type_t get_var_type(const std::string& var_name)
{
#ifndef NDEBUG
	std::cout << "looking up type of " << var_name << std::endl;
#endif
	return Systems::instance().get_root()->get_variable(var_name).get_type();
}


type_t subtree_type(tree_iter_t const& i)
{
	// at the leaf nodes, the type is immediately available
	if(i->value.id() == new_command_parser::integerID)
		return int32;
	if(i->value.id() == new_command_parser::realID)
		return real;
	if(i->value.id() == new_command_parser::complexID)
		return complex;
	if(i->value.id() == new_command_parser::stringID)
		return string;
	if(i->value.id() == new_command_parser::identifierID)
		return get_var_type(make_string(i));

	std::set< type_t > types;
	// descend further into the tree and collect types
	for(tree_iter_t branch_it = i->children.begin(); branch_it != i->children.end(); ++branch_it)
	{
		types.insert(subtree_type(branch_it));
	}

	if(types.find(string) != types.end())
		return string;
	if(types.find(complex) != types.end())
		return complex;
	if(types.find(real) != types.end())
		return real;
	if(types.find(int32) != types.end())
		return int32;

	return empty;
}



template< typename T >
T make_scalar(tree_iter_t const& i);

template< >
int32_t make_scalar(tree_iter_t const& i)
{
	std::string integer(i->value.begin(), i->value.end());
	return strtol(integer.c_str(), 0, 10);
}

template< >
real_t make_scalar(tree_iter_t const& i)
{
	std::string realnum(i->value.begin(), i->value.end());
	return strtod(realnum.c_str(), 0);
}

template< >
complex_t make_scalar(tree_iter_t const& i)
{
	std::string realnum(i->value.begin(), i->value.end());

	if(realnum == "i" || realnum == "j")
		return complex_t(0,1);
	else
		return complex_t(strtod(realnum.c_str(), 0));
}




template< typename T >
T evaluate_scalar_arithmetic(tree_iter_t const& i)
{
	if
	(
		(i->value.id() == new_command_parser::integerID) ||
		(i->value.id() == new_command_parser::realID) ||
		(i->value.id() == new_command_parser::complexID)
	)
	{
		assert(i->children.size() == 0);
		return make_scalar< T >(i);
	}
	else
	if(i->value.id() == new_command_parser::scalarexpressionID)
	{
		if (*i->value.begin() == '+')
		{
			assert(i->children.size() == 2);
			return evaluate_scalar_arithmetic< T >(i->children.begin()) +
				evaluate_scalar_arithmetic< T >(i->children.begin()+1);
		}
		else if (*i->value.begin() == '-')
		{
			assert(i->children.size() == 2);
			return evaluate_scalar_arithmetic< T >(i->children.begin()) -
				evaluate_scalar_arithmetic< T >(i->children.begin()+1);
		}
	}
	else
	if(i->value.id() == new_command_parser::scalartermID)
	{
		if (*i->value.begin() == '*')
		{
			assert(i->children.size() == 2);
			return evaluate_scalar_arithmetic< T >(i->children.begin()) *
				evaluate_scalar_arithmetic< T >(i->children.begin()+1);	
		}
		else if (*i->value.begin() == '/')
		{
			assert(i->children.size() == 2);
			return evaluate_scalar_arithmetic< T >(i->children.begin()) /
				evaluate_scalar_arithmetic< T >(i->children.begin()+1);
		}
	}
	else
	if (i->value.id() == new_command_parser::scalarfactorID)
	{
		assert(*i->value.begin() == '-');
		return - evaluate_scalar_arithmetic< T >(i->children.begin());
	}
	assert(0);
	return 0;
}


template< typename T >
void make_range(T start, T end, T inc, Variable& v)
{
	assert(start * inc < end);

	size_t elements = static_cast< size_t >(floor((end-start) / inc +1));
	v.prealloc(sizeof(T) * (elements + v.size()));
#ifndef NDEBUG
	std::cout << "elements: " << elements << std::endl;
#endif
	T f = start;

	for(size_t i=0; i<elements; ++i)
	{
		v.push_back(f);
		f += inc;
	}
}


template< typename T >
void make_array(tree_iter_t const&  start, tree_iter_t const& end, Variable& v)
{
	tree_iter_t curr = start;
	size_t elements = end - start;
	v.prealloc(sizeof(T) * (elements + v.size()));

	for(size_t i=0; i < elements; ++i)
	{
		v.push_back(evaluate_scalar_arithmetic< T >(curr));
		++curr;
	}
}


void eval_expression(tree_iter_t const& i, Variable& v)
{
	if(i->value.id() == new_command_parser::identifierID)
	{
		assert(i->children.size() == 0);
		v = Systems::instance().get_root()->get_variable(make_string(i));
		return;
	}
	else
	if
	(
		i->value.id() == new_command_parser::scalarexpressionID ||
		i->value.id() == new_command_parser::scalarfactorID ||
		i->value.id() == new_command_parser::scalartermID ||
		i->value.id() == new_command_parser::integerID ||
		i->value.id() == new_command_parser::realID ||
		i->value.id() == new_command_parser::complexID
	)
	{
		type_t num_type = subtree_type(i);
#ifndef NDEBUG
		std::cout << "scalar arithmetic" << std::endl;
		std::cout << "  type: " << num_type << std::endl;
#endif
		switch(num_type)
		{
			case int32:
				v.push_back(evaluate_scalar_arithmetic< int32_t >(i));
				break;
			case real:
				v.push_back(evaluate_scalar_arithmetic< real_t >(i));
				break;
			case complex:
				v.push_back(evaluate_scalar_arithmetic< complex_t >(i));
				break;
			default:
				assert(0);
		}
	}
	else
	if(i->value.id() == new_command_parser::arrayID)
	{
		assert(i->children.size() > 1);
#ifndef NDEBUG
		std::cout << "array" << std::endl;
		std::cout << "  rows: " << i->children.size() << std::endl;
		std::cout << "  columns: " << i->children.begin()->children.size() << std::endl;
		std::cout << "  numeric type: " << subtree_type(i) << std::endl;
#endif
	}
	else
	if(i->value.id() == new_command_parser::rowvecID)
	{
		assert(i->children.size() > 1);
#ifndef NDEBUG
		std::cout << "  row_vec" << std::endl;
		std::cout << "    elements: " << i->children.size() << std::endl;
		std::cout << "    numeric type: " << subtree_type(i) << std::endl;
#endif
		if(subtree_type(i) == real)
		{
			make_array< real_t >(i->children.begin(), i->children.end(), v);
		}
		if(subtree_type(i) == int32)
		{
			make_array< int32_t >(i->children.begin(), i->children.end(), v);
		}
		if(subtree_type(i) == complex)
		{
			make_array< complex_t >(i->children.begin(), i->children.end(), v);
		}
	}
	else
	if(i->value.id() == new_command_parser::stringID)
	{
		assert(i->children.size() == 0);

		v.push_back(std::string(i->value.begin(), i->value.end()));
	}
	else
	if(i->value.id() == new_command_parser::rangeID)
	{
		assert(i->children.size() == 3);
#ifndef NDEBUG
		std::cout << "range" << std::endl;
		std::cout << "  numeric type: " << subtree_type(i) << std::endl;
#endif
		if(subtree_type(i) == real)
		{
			make_range< real_t >
			(
				evaluate_scalar_arithmetic< real_t >(i->children.begin()),
				evaluate_scalar_arithmetic< real_t >(i->children.begin()+2),
				evaluate_scalar_arithmetic< real_t >(i->children.begin()+1),
				v
			);
		} else
		if(subtree_type(i) == int32)
		{
			make_range< int32_t >
			(
				evaluate_scalar_arithmetic< int32_t >(i->children.begin()),
				evaluate_scalar_arithmetic< int32_t >(i->children.begin()+2),
				evaluate_scalar_arithmetic< int32_t >(i->children.begin()+1),
				v
			);
		}
	}
	else assert(0);
}


void eval_assignment(tree_iter_t const& i)
{
	assert(i->children.size() == 2);

	std::string var_name(make_string(i->children.begin()));
#ifndef NDEBUG
	std::cout << "assignment" << std::endl;
	std::cout << "  variable name: " << var_name << std::endl;
	std::cout << "  data type: " << subtree_type(i->children.begin()+1) << std::endl;
#endif
	Variable var;
	eval_expression(i->children.begin()+1, var);
	Systems::instance().get_root()->add_variable(var_name, var);
}


void eval_add_block(tree_iter_t const& i)
{
	assert(i->children.size() >= 2);

	std::string block_name, block_type;

	uint32_t num_args = i->children.size()-2;
	block_type = make_string(i->children.begin());
	block_name = make_string(i->children.begin()+1);
#ifndef NDEBUG
	std::cout << "adding block" << std::endl;
	std::cout << "  type: " << block_type << std::endl;
	std::cout << "  name: " << block_name << std::endl;
	std::cout << "  no. of arguments given: " << num_args << std::endl;
#endif

	Block *b = BlockLoader::instance().new_block(block_type);

	if(b->get_params().size() != num_args)
	{
		std::string bad_block_name = b->get_name_sys();
		delete b;
		// checked ok. does not leak memory
		throw ParameterCountMismatchException(bad_block_name);
	}

	// fill block with provided parameters
	for(uint32_t param_num=0; param_num<num_args; ++param_num)
	{
		Variable param_curr;
#ifndef NDEBUG
		std::cout << "  parameter type: " << subtree_type(i->children.begin()+2+param_num) << std::endl;
#endif
		eval_expression(i->children.begin() + param_num + 2, param_curr);

		assert(subtree_type(i->children.begin()+2+param_num) == param_curr.get_type());
		assert(param_curr);

		b->set_parameter(param_curr);
	}
	assert(b->is_configured());

	Systems::instance().get_root()->add_block(b, block_name);
}


boost::spirit::parser_id identify_line(tree_iter_t const& i)
{
	return i->value.id();
}


void eval_connect(tree_iter_t const& i)
{
	assert(i->children.size() == 4);

	std::string source_block_name, sink_block_name, source_port_name, sink_port_name;
#ifndef NDEBUG
	std::cout << "connecting blocks ";
#endif
	source_block_name = make_string(i->children.begin());
	source_port_name = make_string(i->children.begin()+1);
	sink_block_name = make_string(i->children.begin()+2);
	sink_port_name = make_string(i->children.begin()+3);
#ifndef NDEBUG
	std::cout << source_block_name << ":" << source_port_name << "->"
		<< sink_block_name << ":" << sink_port_name << std::endl;
#endif
	Systems::instance().get_root()->connect_ports(source_block_name, source_port_name, sink_block_name, sink_port_name);
}


void eval_run(tree_iter_t const& i)
{
	assert(i->children.size() <= 2);
	int32_t times = 1;

	if(i->children.size() == 2)
	{
		Variable v;
		eval_expression(i->children.begin()+1, v);
		if(v.get_type() == int32)
		{
			variable_iterator< int32_t > vi(v);
			times = *vi;
		}
	}
#ifndef NDEBUG
	std::cout << "running system " << times << " times" << std::endl;
	std::cout << "initializing..." << std::endl;
#endif
	Systems::instance().get_root()->initialize();
#ifndef NDEBUG
	std::cout << "starting system..." << std::endl;
#endif
	Systems::instance().get_root()->wakeup_sys(times);
}


#endif
