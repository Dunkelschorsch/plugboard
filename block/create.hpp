#ifndef BLOCK_CREATE_HPP
#define BLOCK_CREATE_HPP


#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "visibility.hpp"


#define ACCESS_FUNCS(NAME)	\
extern "C"			\
{				\
DSOEXPORT Block* create();	\
DSOEXPORT const string_t name();\
}				\
				\
Block* create() 		\
{				\
	return new BOOST_PP_CAT(Block_, NAME);\
}				\
				\
const string_t name()		\
{				\
	return BOOST_PP_STRINGIZE(NAME);	\
}


#endif // BLOCK_CREATE_HPP
