#ifndef FLOAT_HPP
#define FLOAT_HPP

#include "grammar/result_closure.hpp"
#include "types/base.hpp"



namespace
{

using namespace boost::spirit;
using phoenix::arg1;
using phoenix::construct_;

struct FLOAT : grammar< FLOAT, ResultClosure< complex_t >::context_t >
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

        definition(FLOAT const& self);
    };
} FLOAT_g;



template< typename ScannerT >
FLOAT::definition< ScannerT >::definition(FLOAT const& self)
{
    main = strict_real_p[ self.result_ = construct_< complex_t >(arg1) ];
    BOOST_SPIRIT_DEBUG_RULE(main);
}

} // namespace

#endif // FLOAT_HPP
