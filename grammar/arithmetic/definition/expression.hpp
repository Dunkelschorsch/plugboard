#ifndef EXPRESSION_DEF_HPP
#define EXPRESSION_DEF_HPP

#include "grammar/arithmetic/term.hpp"


namespace
{

using phoenix::arg1;

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

}

#endif // EXPRESSION_DEF_HPP
