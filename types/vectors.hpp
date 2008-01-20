#ifndef VECTORS_HPP
#define VECTORS_HPP

#include "types/base.hpp"

#define BOOST_PP_DEF(z, I, _) \
	typedef std::vector< BOOST_PP_ARRAY_ELEM(0, SIGNAL_TYPE(I)) > \
		BOOST_PP_CAT(BOOST_PP_ARRAY_ELEM(1, SIGNAL_TYPE(I)), _vec_t);

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF

#endif // VECTORS_HPP
