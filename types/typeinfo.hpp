#ifndef TYPEINFO_HPP
#define TYPEINFO_HPP

#include "types/base.hpp"

// declare neccessary signal classes
#define BOOST_PP_DEF(z, I, _) \
	class SIG_TYPE(I); \

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF

namespace
{

template< typename T >
struct typeinfo;

#define BOOST_PP_DEF(z, I, _) \
	template< > \
	struct typeinfo< CPP_TYPE(I) > \
	{	\
		typedef SIG_TYPE(I) signal_type; \
		typedef VEC_TYPE(I) vector_type; \
		static const type_t value = TYPE_VALUE(I); \
		static const size_t size = sizeof(CPP_TYPE(I)); \
	};

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF

}

#endif // TYPEINFO_HPP
