#ifndef FACTOR_DEF_HPP
#define FACTOR_DEF_HPP

#include "grammar/literal/integer.hpp"
#include "grammar/literal/float.hpp"
#include "grammar/literal/imaginary.hpp"
#include "grammar/arithmetic/expression.hpp"


namespace
{

using phoenix::arg1;

template< typename ScannerT >
FACTOR::definition< ScannerT >::definition(FACTOR const& self)
{
    main =
        ( submain
              =    group
                   | IMAG_g    [ self.result_ = arg1 ]
                   | FLOAT_g   [ self.result_ = arg1 ]
                   | INTEGER_g [ self.result_ = arg1 ]
              ,
          group
              =    ch_p('(')
                   >> EXPRESSION_g[ self.result_ = arg1 ]
                   >> ch_p(')')
        );

    BOOST_SPIRIT_DEBUG_RULE(main);
    BOOST_SPIRIT_DEBUG_RULE(submain);
    BOOST_SPIRIT_DEBUG_RULE(group);
    BOOST_SPIRIT_DEBUG_NODE(INTEGER_g);
    BOOST_SPIRIT_DEBUG_NODE(FLOAT_g);
    BOOST_SPIRIT_DEBUG_NODE(IMAG_g);
}

}

#endif // FACTOR_DEF_HPP
