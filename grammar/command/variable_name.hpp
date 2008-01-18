#ifndef VARIABLE_NAME_HPP
#define VARIABLE_NAME_HPP

#include "grammar/result_closure.hpp"
#include "grammar/command/identifier.hpp"



namespace
{

using namespace boost::spirit;
using phoenix::arg1;
using phoenix::construct_;

struct VARIABLE_NAME : grammar< VARIABLE_NAME, ResultClosure< std::string >::context_t >
{
    template< typename ScannerT >
    struct definition {
        typedef rule< ScannerT > rule_t;
        rule_t main;

        rule_t const& start() const
        {
            return main;
        }

        definition(VARIABLE_NAME const& self);
    };
} VARIABLE_NAME_g;


template< typename ScannerT >
VARIABLE_NAME::definition< ScannerT >::definition(VARIABLE_NAME const& self)
{
    main =
        lexeme_d
        [
            ch_p('$')
            >> IDENTIFIER_g[ self.result_ = construct_< std::string >(arg1) ]
        ]
        ;
    BOOST_SPIRIT_DEBUG_RULE(main);
}

} // namespace


#endif // VARIABLE_NAME_HPP
