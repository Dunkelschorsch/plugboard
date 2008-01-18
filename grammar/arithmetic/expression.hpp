#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "grammar/result_closure.hpp"
#include "types/base.hpp"


namespace
{

using namespace boost::spirit;

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

} // namespace

#include "definition/expression.hpp"

#endif // EXPRESSION_HPP
