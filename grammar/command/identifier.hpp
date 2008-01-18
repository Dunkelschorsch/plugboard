#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include "grammar/result_closure.hpp"



namespace
{

using namespace boost::spirit;
using phoenix::arg1;
using phoenix::arg2;
using phoenix::construct_;

struct IDENTIFIER : grammar< IDENTIFIER, ResultClosure< std::string >::context_t >
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

        definition(IDENTIFIER const& self);
    };
} IDENTIFIER_g;



template< typename ScannerT >
IDENTIFIER::definition< ScannerT >::definition(IDENTIFIER const& self)
{
    main = (
        lexeme_d
        [
            (+(alnum_p | '_'))
            [
                self.result_ = construct_< std::string >(arg1, arg2)
            ]
        ]
    );
    BOOST_SPIRIT_DEBUG_RULE(main);
}

} // namespace


#endif // IDENTIFIER_HPP
