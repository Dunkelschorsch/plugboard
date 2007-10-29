#ifndef _VARIABLE_PARSER_HPP
#define _VARIABLE_PARSER_HPP

#include <string>
#include <vector>
#include <algorithm>

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/spirit/dynamic/for.hpp>
#include <boost/spirit/actor/increment_actor.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/any.hpp>

#ifndef NDEBUG
#include <iostream>
#endif

#include "variable.hpp"
#include "types.hpp"

using namespace boost::spirit;
using boost::ref;



struct value_closure : boost::spirit::closure< value_closure, complex_t >
{
    member1 val;
};



struct var_closure : boost::spirit::closure< var_closure, Variable >
{
    member1 v;
};



template< class ActorT >
struct ValueAppendAction
{
    ValueAppendAction(const ActorT& v) : v_(v) { }

    template< typename ValueT >
    void operator()(const ValueT& val_curr) const
    {
#ifndef NDEBUG
        std::cout << "current value: " << val_curr << std::endl;
#endif
        if(val_curr.imag() == 0)
        {
#ifndef NDEBUG
            std::cout << "variable is not complex." << std::endl;
#endif
            if(static_cast< integer_t >(val_curr.real()) == val_curr.real())
            {
#ifndef NDEBUG
                std::cout << "variable is integer valued." << std::endl;
#endif
                v_().append(static_cast< integer_t >(val_curr.real()));
                v_().save_type_change(integer);
            }
            else
            {
#ifndef NDEBUG
                std::cout << "variable is real valued." << std::endl;
#endif
                v_().append(static_cast< real_t >(val_curr.real()));
                v_().save_type_change(real);
            }
        }
        else
        {
#ifndef NDEBUG
            std::cout << "variable is complex." << std::endl;
#endif
            v_().append(val_curr);
            v_().save_type_change(complex);
        }
#ifndef NDEBUG
        std::cout << "type of variable: " << v_().get_type() << std::endl;
#endif
    }

    const ActorT& v_;
};



template< class ActorT >
inline ValueAppendAction< ActorT > append_value_a(ActorT& v)
{
    return ValueAppendAction< ActorT >(v);
}



template< class ActorT, typename IntT >
struct DimensionAddAction
{
    DimensionAddAction(const ActorT& v, const IntT size) : v_(v), size_(size) { }

    template< typename WhatverT >
    void operator()(const WhatverT=0, const WhatverT=0) const
    {
        v_().add_dimension(size_);
    }

    const ActorT& v_;
    const IntT size_;
};



template< class ActorT, typename IntT >
inline DimensionAddAction< ActorT, IntT > add_dimension_a(const ActorT& v, const IntT size)
{
    return DimensionAddAction< ActorT, IntT >(v, size);
}



struct VariableParser : public grammar< VariableParser, var_closure::context_t >
{
    template < typename ScannerT >
    struct definition
    {
        uint16_t count;
        uint16_t dim1, dim2;

        definition(VariableParser const& self)
        {
            using namespace phoenix;
            first = (
                init
                    =    eps_p /* initialisation */
                             [self.v=construct_< Variable >()]
                             [var(count)=0]
                             [var(dim1)=0]
                             [var(dim2)=0]
                         >> expression
                                [append_value_a(self.v)]
                                [add_dimension_a(self.v, 1)]
                                [add_dimension_a(self.v, 1)]
                         | list
                               [add_dimension_a(self.v, ref(dim1))]
                               [add_dimension_a(self.v, ref(dim2))]
                    ,

                list
                    =    ch_p("[")
                         >> !list_head
                             [
                                 var(dim1)++
                             ]
                         >> ch_p("]")
                    ,

                list_head
                    =    expression
                             [var(dim2)++]
                             [append_value_a(self.v)]
                         >>   *(   !ch_p(",")
                                   >> expression
                                      [
                                          append_value_a(self.v)
                                      ]
                               )
                               [
                                   var(dim2)++
                               ]
                         >>   *(   ch_p(";")
                                   >> list_tail
                                      [
                                          var(dim1)++
                                      ]
                               )
                    ,

                list_tail
                    =    for_p
                         (
                             var(count)=0,
                             var(count)<var(dim2),
                             var(count)++
                         )
                         [
                             expression
                             [
                                 append_value_a(self.v)
                             ]
                             >> !ch_p(",")
                         ]
                    ,

                group
                    =    '('
                         >> expression
                            [
                                factor.val=arg1
                            ]
                         >> ')'
                    ,

                factor
                    =      group
                         | imag
                         | real_num
                         | int_num
                    ,

                 term
                    =    factor
                         [
                             term.val=arg1
                         ]
                         >>  *( (  '*'
                                   >> factor
                                      [
                                          term.val*=arg1
                                      ]
                                )
                              | (  '/'
                                   >> factor
                                      [
                                          term.val/=arg1
                                      ]
                                )
                              )
                    ,

                expression
                    =    term
                         [
                             expression.val=arg1
                         ]
                         >>  *( (  '+'
                                    >> term
                                       [
                                           expression.val+=arg1
                                       ]
                                )
                              | (  '-'
                                   >> term
                                      [
                                          expression.val-=arg1
                                      ]
                                )
                              )
                    ,

                real_num
                    =    strict_real_p
                         [
                             factor.val=construct_< complex_t >(arg1)
                         ]
                    ,

                int_num
                    =    int_p
                         [
                             factor.val=construct_< complex_t >(arg1)
                         ]
                    ,

                imag
                    =    lexeme_d
                         [
                             real_p
                             [
                                 factor.val=construct_< complex_t >(0,arg1)
                             ]
                             >> !ch_p('*') 
                             >>  ch_p("j")
                         ]
                         |   ch_p("j")
                             [
                                 factor.val=construct_< complex_t >(0,1)
                             ]
                    );

            BOOST_SPIRIT_DEBUG_RULE(self);
            BOOST_SPIRIT_DEBUG_RULE(init);
            BOOST_SPIRIT_DEBUG_RULE(first);
            BOOST_SPIRIT_DEBUG_RULE(list);
            BOOST_SPIRIT_DEBUG_RULE(list_head);
            BOOST_SPIRIT_DEBUG_RULE(list_tail);
            BOOST_SPIRIT_DEBUG_RULE(factor);
            BOOST_SPIRIT_DEBUG_RULE(group);
            BOOST_SPIRIT_DEBUG_RULE(term);
            BOOST_SPIRIT_DEBUG_RULE(int_num);
            BOOST_SPIRIT_DEBUG_RULE(real_num);
            BOOST_SPIRIT_DEBUG_RULE(imag);
            BOOST_SPIRIT_DEBUG_RULE(expression);
        }

        rule< ScannerT > first;

        subrule< 0 > init;
        subrule< 1 > list;
        subrule< 2 > list_head;
        subrule< 3 > list_tail;
        subrule< 4, value_closure::context_t > factor;
        subrule< 5, value_closure::context_t > term;
        subrule< 6, value_closure::context_t > expression;
        subrule< 7 > int_num;
        subrule< 8 > real_num;
        subrule< 9 > imag;
        subrule< 10 > group;

        rule< ScannerT > const& start() const
        {
            return first;
        }
    };
};

#undef BOOST_SPIRIT_DEBUG

#endif // _VARIABLE_PARSER_HPP
