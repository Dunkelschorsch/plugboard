#ifndef TERM_DEF_HPP
#define TERM_DEF_HPP

#include "grammar/arithmetic/factor.hpp"


namespace
{

using phoenix::arg1;

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

}

#endif // TERM_DEF_HPP
