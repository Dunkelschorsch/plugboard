#ifndef IMAGINARY_HPP
#define IMAGINARY_HPP

#include "grammar/result_closure.hpp"
#include "types/base.hpp"



namespace
{

using namespace boost::spirit;
using phoenix::arg1;
using phoenix::construct_;

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

} // namespace

#endif // IMAGINARY_HPP
