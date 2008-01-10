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
using phoenix::arg1;
using phoenix::arg2;
using phoenix::construct_;



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


namespace
{


template< typename ResultT >
struct ResultClosure : boost::spirit::closure< ResultClosure< ResultT >, ResultT >
{
    typedef boost::spirit::closure< ResultClosure< ResultT >, ResultT > base_t;
    typename base_t::member1 result_;
};



struct INTEGER : grammar< INTEGER, ResultClosure< complex_t >::context_t >
{
    template< typename ScannerT >
    struct definition
    {
        typedef rule< ScannerT > rule_t;
        rule_t main;

        rule_t const& start() const
        {
            return main;
        }

        definition(INTEGER const& self);
    };
} INTEGER_g;



struct REAL : grammar< REAL, ResultClosure< complex_t >::context_t >
{
    template< typename ScannerT >
    struct definition
    {
        typedef rule< ScannerT > rule_t;
        rule_t main;

        rule_t const& start() const
        {
            return main;
        }

        definition(REAL const& self);
    };
} REAL_g;



struct IMAG : grammar< IMAG, ResultClosure< complex_t >::context_t >
{
    template< typename ScannerT >
    struct definition
    {
        typedef rule< ScannerT > rule_t;
        rule_t main;

        rule_t const& start() const
        {
            return main;
        }

        definition(IMAG const& self);
    };
} IMAG_g;



struct FACTOR : grammar< FACTOR, ResultClosure< complex_t >::context_t >
{
    template< typename ScannerT >
    struct definition
    {
        typedef rule< ScannerT > rule_t;
        rule_t main;

        rule_t const& start() const
        {
            return main;
        }

        subrule< 0 > first;
        subrule< 1 > group;

        definition(FACTOR const& self);
    };
} FACTOR_g;



struct EXPRESSION : grammar< EXPRESSION, ResultClosure< complex_t >::context_t >
{
    template< typename ScannerT >
    struct definition
    {
        typedef rule< ScannerT > rule_t;
        rule_t main;

        rule_t const& start() const
        {
            return main;
        }

        definition(EXPRESSION const& self);
    };
} EXPRESSION_g;



struct TERM : grammar< TERM, ResultClosure< complex_t >::context_t >
{
    template< typename ScannerT >
    struct definition
    {
        typedef rule< ScannerT > rule_t;
        rule_t main;

        rule_t const& start() const
        {
            return main;
        }

        definition(TERM const& self);
    };
} TERM_g;



template< typename ScannerT >
EXPRESSION::definition< ScannerT >::definition(EXPRESSION const& self)
{
    main =
        ( TERM_g[ self.result_ = arg1 ]
          >>  *( ( '+' >> TERM_g[ self.result_ += arg1 ] )
               | ( '-' >> TERM_g[ self.result_ -= arg1 ] )
               )
        );

    BOOST_SPIRIT_DEBUG_RULE(main);
    BOOST_SPIRIT_DEBUG_NODE(TERM_g);
}



template< typename ScannerT >
INTEGER::definition< ScannerT >::definition(INTEGER const& self)
{
    main = int_p[ self.result_ = construct_< complex_t >(arg1) ];
    BOOST_SPIRIT_DEBUG_RULE(main);
}



template< typename ScannerT >
REAL::definition< ScannerT >::definition(REAL const& self)
{
    main = strict_real_p[ self.result_ = construct_< complex_t >(arg1) ];
    BOOST_SPIRIT_DEBUG_RULE(main);
}



template< typename ScannerT >
IMAG::definition< ScannerT >::definition(IMAG const& self)
{
    main
        = lexeme_d
          [
              real_p[ self.result_ = construct_< complex_t >(0, arg1) ]
              >> !ch_p('*') >> ch_p('j')
          ]
          |   ch_p('j')[ self.result_ = construct_< complex_t >(0,1) ]
        ;
    BOOST_SPIRIT_DEBUG_RULE(main);
}



template< typename ScannerT >
FACTOR::definition< ScannerT >::definition(FACTOR const& self)
{
    main =
        ( first
              =    group
                   | IMAG_g    [ self.result_ = arg1 ]
                   | REAL_g    [ self.result_ = arg1 ]
                   | INTEGER_g [ self.result_ = arg1 ]
              ,
          group
              =    ch_p('(')
                   >> EXPRESSION_g[ self.result_ = arg1 ]
                   >> ch_p(')')
        );

    BOOST_SPIRIT_DEBUG_RULE(main);
    BOOST_SPIRIT_DEBUG_RULE(first);
    BOOST_SPIRIT_DEBUG_RULE(group);
    BOOST_SPIRIT_DEBUG_NODE(INTEGER_g);
    BOOST_SPIRIT_DEBUG_NODE(REAL_g);
    BOOST_SPIRIT_DEBUG_NODE(IMAG_g);
}



template< typename ScannerT >
TERM::definition< ScannerT >::definition(TERM const& self)
{
    main =
        (
            FACTOR_g[ self.result_ = arg1 ]
            >>  *( ( '*' >> FACTOR_g[ self.result_ *= arg1 ] )
                 | ( '/' >> FACTOR_g[ self.result_ /= arg1 ] )
                 )
        );

    BOOST_SPIRIT_DEBUG_RULE(main);
    BOOST_SPIRIT_DEBUG_NODE(FACTOR_g);
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
                         >> EXPRESSION_g
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
                    =    EXPRESSION_g
                             [var(dim2)++]
                             [append_value_a(self.v)]
                         >>   *(   !ch_p(",")
                                   >> EXPRESSION_g
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
                             EXPRESSION_g
                             [
                                 append_value_a(self.v)
                             ]
                             >> !ch_p(",")
                         ]
                    );

            BOOST_SPIRIT_DEBUG_RULE(self);
            BOOST_SPIRIT_DEBUG_RULE(init);
            BOOST_SPIRIT_DEBUG_RULE(first);
            BOOST_SPIRIT_DEBUG_RULE(list);
            BOOST_SPIRIT_DEBUG_RULE(list_head);
            BOOST_SPIRIT_DEBUG_RULE(list_tail);
            BOOST_SPIRIT_DEBUG_NODE(EXPRESSION_g);
        }

        rule< ScannerT > first;

        subrule< 0 > init;
        subrule< 1 > list;
        subrule< 2 > list_head;
        subrule< 3 > list_tail;

        rule< ScannerT > const& start() const
        {
            return first;
        }
    };
} VARIABLE_g;

}


#endif // _VARIABLE_PARSER_HPP
