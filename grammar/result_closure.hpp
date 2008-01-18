#ifndef RESULT_CLOSURE_HPP
#define RESULT_CLOSURE_HPP

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>



namespace
{

template< typename ResultT >
struct ResultClosure : boost::spirit::closure< ResultClosure< ResultT >, ResultT >
{
    typedef boost::spirit::closure< ResultClosure< ResultT >, ResultT > base_t;
    typename base_t::member1 result_;
};

}

#endif // RESULT_CLOSURE_HPP
