#ifndef _TYPEINFO_HPP
#define _TYPEINFO_HPP

#include "types/base.hpp"

namespace
{

template< typename T >
struct typeinfo;

#define BOOST_PP_DEF(z, I, _) \
	template< > \
	struct  typeinfo< BOOST_PP_ARRAY_ELEM(0, SIGNAL_TYPE(I)) > \
	{	\
		static const type_t value = BOOST_PP_ARRAY_ELEM(1, SIGNAL_TYPE(I)); \
		static const size_t size = sizeof(BOOST_PP_ARRAY_ELEM(0, SIGNAL_TYPE(I))); \
	};

BOOST_PP_REPEAT_FROM_TO(0, BOOST_PP_SUB(SIGNAL_TYPE_CNT, 1), BOOST_PP_DEF, _)

#undef BOOST_PP_DEF

}

#endif // _TYPEINFO_HPP
