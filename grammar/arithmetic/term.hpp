#ifndef TERM_HPP
#define TERM_HPP

#include "grammar/result_closure.hpp"
#include "types/base.hpp"


namespace
{

using namespace boost::spirit;

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

} // namespace

#include "definition/term.hpp"

#endif // TERM_HPP
