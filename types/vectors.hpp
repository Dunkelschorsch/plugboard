#ifndef VECTORS_HPP
#define VECTORS_HPP

#include "types/base.hpp"
#include <vector>

namespace plugboard
{
	#define BOOST_PP_DEF(z, I, _) \
		typedef std::vector< CPP_TYPE(I) > \
			BOOST_PP_CAT(TYPE_VALUE(I), _vec_t);

	BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

	#undef BOOST_PP_DEF
}

#endif // VECTORS_HPP
