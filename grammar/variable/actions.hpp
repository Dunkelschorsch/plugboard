#ifndef VARIABLE_ACTIONS_HPP
#define VARIABLE_ACTIONS_HPP

#include <boost/spirit/phoenix/actor.hpp>
#include <boost/spirit/phoenix/closures.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/iterator/position_iterator.hpp>

#include "variable/variable.hpp"



typedef phoenix::actor<
    phoenix::closure_member<
        0, phoenix::closure<
            Variable, phoenix::nil_t, phoenix::nil_t
        >
    >
> variable_actor;

typedef complex_t variable_value_t;


struct ValueAppendAction
{
    typedef void result_type;

    ValueAppendAction(const variable_actor&);
    result_type operator()(const variable_value_t&) const;

    const variable_actor& v_;
};



struct DimensionAddAction
{
    typedef void result_type;

    DimensionAddAction(const variable_actor&, size_t);

    result_type operator()(const complex_t&) const;

    // i don't have the slightest clue why this overload is neccessary
    template< typename StangeIteratorT >
    result_type operator()(const StangeIteratorT&, const StangeIteratorT&) const
    {
#ifndef NDEBUG
        std::cout << "BLOODY HELL!!" << std::endl;
        bool spirit_does_not_suck = false;
        assert(spirit_does_not_suck);
#endif
    }

    const variable_actor& v_;
    const size_t size_;
};


#endif // VARIABLE_ACTIONS_HPP
