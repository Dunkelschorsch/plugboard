#ifndef BLOCK_CREATE_HPP
#define BLOCK_CREATE_HPP

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>


#define ACCESS_FUNCS(NAME)	\
extern "C" {			\
Block* create()			\
{				\
	return new BOOST_PP_CAT(Block_, NAME);\
}				\
const string_t name()		\
{				\
	return BOOST_PP_STRINGIZE(NAME);	\
} }

#endif // BLOCK_CREATE_HPP
