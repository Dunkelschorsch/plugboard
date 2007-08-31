
//#define BOOST_SPIRIT_DEBUG
#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/spirit/dynamic/for.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/increment_actor.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign.hpp>
#include "variable.hpp"
#include "types.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


using namespace boost::spirit;
using boost::bind;
using boost::ref;


struct value_closure : boost::spirit::closure< value_closure, real_t >
{
	member1 val;
};

struct var_closure : boost::spirit::closure< var_closure, Variable >
{
	member1 v;
};


struct variable : public grammar< variable, var_closure::context_t >
{

	template < typename ScannerT >
	struct definition
	{
		boost::function< void(real_t) > push;

		uint16_t count;
		uint16_t dim1, dim2;
;
		definition(variable const& self)
		{
			push = bind(&Variable::app, ref(self.v()), _1);
			
			using namespace phoenix;
			first =
			(
			init
			    =	eps_p /* initialisation */
				[self.v=construct_< Variable >()]
				[var(count)=0]
				[var(dim1)=0]
				[var(dim2)=0]	
				>> expression [push]
			    |	list
				[bind(&Variable::add_dimension, ref(self.v()), ref(dim1))]
				[bind(&Variable::add_dimension, ref(self.v()), ref(dim2))]
			    ,

			list
			    =	ch_p("[")
			     	>> !list_head [var(dim1)++]
			     	>> ch_p("]")
			    ,

			list_head
			    =	expression [var(dim2)++] [push]
				>> *(
			        	!ch_p(",")
					>> expression [push]
				    ) [var(dim2)++]
				>> *(
					ch_p(";")
					>> list_tail [var(dim1)++]
				    )
			    ,
			
			list_tail
			    =	for_p(var(count)=0, var(count)<var(dim2), var(count)++) [expression[push] >> !ch_p(",")]
			    ,

			scalar
			    =	real_num | int_num
			    ,

			expression
			    =	scalar[expression.val=arg1]
				>> *(
					ch_p("+") >> scalar[expression.val+=arg1]
				    |	ch_p("-") >> scalar[expression.val-=arg1]
				    )
			    ,

			real_num
			    =	strict_real_p[scalar.val=arg1]
			    ,

			int_num
			    =	int_p[scalar.val=arg1]
			)

			BOOST_SPIRIT_DEBUG_RULE(list);
			BOOST_SPIRIT_DEBUG_RULE(scalar);
			BOOST_SPIRIT_DEBUG_RULE(list_tail);
			BOOST_SPIRIT_DEBUG_RULE(real_num);
			BOOST_SPIRIT_DEBUG_RULE(int_num);
			BOOST_SPIRIT_DEBUG_RULE(first);
			BOOST_SPIRIT_DEBUG_RULE(expression);
			BOOST_SPIRIT_DEBUG_RULE(list_head);
		}

		rule< ScannerT > first;

		subrule< 0 > init;
		subrule< 1 > list;
		subrule< 2 > list_head;
		subrule< 3 > list_tail;
		subrule< 4, value_closure::context_t > expression;
		subrule< 5, value_closure::context_t > scalar;
		subrule< 6 > int_num;
		subrule< 7 > real_num;

		rule< ScannerT > const& start() const
		{
			return first;
		}
	};

};


int
main()
{
	variable h ;    //  Our parser
	Variable v;
	std::string str = "[1 2 3 4 ; 1 7 2 14  ; 4  1 1 1]";

	parse_info<> info;
	for(int i=0; i<20000; i++)
	{
	
	
	info = parse(str.c_str(), h[assign(v)], space_p);
}
	if (info.full)
	{
		std::cout << "-------------------------\n";
		std::cout << "Parsing succeeded\n";
		std::cout << "-------------------------\n";

	std::cout << "[ ";
	for(Variable::iterator it = v.begin(); it!=v.end(); it++)
	{
		std::cout << boost::any_cast< real_t >(*it) << " " ;
	}
	std::cout << "]" << std::endl;

	std::vector< short unsigned int > k = v.dims();

	std::cout << k[0] << "x" << k[1] << ":" << v.size() << std::endl;
	}

	else
	{
		std::cout << "-------------------------\n";
		std::cout << "Parsing failed\n";
		std::cout << "stopped at: \"" << info.stop << "\"\n";
		std::cout << "-------------------------\n";
	}



	return 0;
}
