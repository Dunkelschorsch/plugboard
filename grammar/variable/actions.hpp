#ifndef VARIABLE_ACTIONS_HPP
#define VARIABLE_ACTIONS_HPP

#include <boost/spirit/phoenix/actor.hpp>
#include <boost/spirit/phoenix/closures.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include "variable/variable.hpp"


typedef boost::spirit::file_iterator< char > iterator_t;

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

    /* i don't have the slightest clue why this over is neccessary */
    result_type operator()(const iterator_t&, const iterator_t&) const;

    const variable_actor& v_;
    const size_t size_;
};


#endif // VARIABLE_ACTIONS_HPP
