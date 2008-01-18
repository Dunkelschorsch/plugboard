#ifndef VARIABLE_PARSER_HPP
#define VARIABLE_PARSER_HPP

#ifndef NDEBUG
#include <iostream>
#endif

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/spirit/dynamic/for.hpp>

#include "variable/variable.hpp"
#include "grammar/arithmetic/expression.hpp"
#include "grammar/variable/actions.hpp"


using namespace boost::spirit;
using boost::ref;
using phoenix::construct_;



struct var_closure : boost::spirit::closure< var_closure, Variable >
{
    member1 v;
};



namespace
{

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
                submain
                    =    eps_p /* initialisation */
                             [self.v=construct_< Variable >()]
                             [var(count)=0]
                             [var(dim1)=0]
                             [var(dim2)=0]
                         >> EXPRESSION_g
                                [ValueAppendAction(self.v)]
                                [DimensionAddAction(self.v, 1)]
                                [DimensionAddAction(self.v, 1)]
                         | list
                               [DimensionAddAction(self.v, ref(dim1))]
                               [DimensionAddAction(self.v, ref(dim2))]
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
                             [ValueAppendAction(self.v)]
                         >>   *(   !ch_p(",")
                                   >> EXPRESSION_g
                                      [
                                          ValueAppendAction(self.v)
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
                                 ValueAppendAction(self.v)
                             ]
                             >> !ch_p(",")
                         ]
                    );

            BOOST_SPIRIT_DEBUG_RULE(self);
            BOOST_SPIRIT_DEBUG_RULE(submain);
            BOOST_SPIRIT_DEBUG_RULE(first);
            BOOST_SPIRIT_DEBUG_RULE(list);
            BOOST_SPIRIT_DEBUG_RULE(list_head);
            BOOST_SPIRIT_DEBUG_RULE(list_tail);
            BOOST_SPIRIT_DEBUG_NODE(EXPRESSION_g);
        }

        rule< ScannerT > first;

        subrule< 0 > submain;
        subrule< 1 > list;
        subrule< 2 > list_head;
        subrule< 3 > list_tail;

        rule< ScannerT > const& start() const
        {
            return first;
        }
    };
} VARIABLE_g;

} // namespace

#endif // VARIABLE_PARSER_HPP
