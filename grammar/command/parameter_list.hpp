#ifndef PARAMETER_LIST_HPP
#define PARAMETER_LIST_HPP

#include "grammar/result_closure.hpp"
#include "grammar/variable/variable_parser.hpp"
#include "grammar/command/variable_name.hpp"
#include "grammar/command/actions.hpp"

#include <vector>
#include <boost/any.hpp>



namespace
{

using namespace boost::spirit;

typedef std::vector< boost::any > param_list_t;


struct PARAMETER_LIST : grammar< PARAMETER_LIST, ResultClosure< param_list_t >::context_t >
{
    template< typename ScannerT >
    struct definition {
        typedef rule< ScannerT > rule_t;
        rule_t main;

        rule_t const& start() const
        {
            return main;
        }

        definition(PARAMETER_LIST const& self);
    };
} PARAMETER_LIST_g;



template< typename ScannerT >
PARAMETER_LIST::definition< ScannerT >::definition(PARAMETER_LIST const& self)
{
    main
        =
            ch_p('(')
            >> !(
                   ( VARIABLE_NAME_g[ var_lookup_a(self.result_()) ]
                   | VARIABLE_g[ push_back_a(self.result_()) ]
                   )  % ch_p(',')
                )
            >> ch_p(')')
        ;
    BOOST_SPIRIT_DEBUG_RULE(main);
}

} // namespace


#endif // PARAMETER_LIST_HPP
