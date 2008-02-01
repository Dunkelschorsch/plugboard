#ifndef BLOCK_CREATE_HPP
#define BLOCK_CREATE_HPP

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#define DLLEXPORT __attribute__ ((visibility("default")))
#define DLLLOCAL  __attribute__ ((visibility("hidden")))


#define ACCESS_FUNCS(NAME)	\
extern "C"			\
{				\
DLLEXPORT Block* create();	\
DLLEXPORT const string_t name();\
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
