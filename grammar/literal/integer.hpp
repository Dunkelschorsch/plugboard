#ifndef INTEGER_HPP
#define INTEGER_HPP

#include "grammar/result_closure.hpp"
#include "types/base.hpp"



namespace
{

using namespace boost::spirit;
using phoenix::arg1;
using phoenix::construct_;

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



template< typename ScannerT >
INTEGER::definition< ScannerT >::definition(INTEGER const& self)
{
    main = int_p[ self.result_ = construct_< complex_t >(arg1) ];
    BOOST_SPIRIT_DEBUG_RULE(main);
}

} // namespace

#endif // INTEGER_HPP
