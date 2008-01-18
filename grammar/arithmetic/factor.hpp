#ifndef FACTOR_HPP
#define FACTOR_HPP

#include "grammar/result_closure.hpp"
#include "types/base.hpp"


namespace
{

using namespace boost::spirit;

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

        subrule< 0 > submain;
        subrule< 1 > group;

        definition(FACTOR const& self);
    };
} FACTOR_g;

} // namespace

#include "definition/factor.hpp"

#endif // FACTOR_HPP
