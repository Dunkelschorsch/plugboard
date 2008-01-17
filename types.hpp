#ifndef _TYPES_HPP
#define _TYPES_HPP

#include <complex>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/comparison/less.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/arithmetic/add.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/empty.hpp>


/** \file types.hpp

   \brief A documented file
*/


#define SIGNAL_TYPE(I) BOOST_PP_CAT(SIGNAL_TYPE, I)

//			name of C++ type	name in type_t	signal name	typedef'ed from
#define SIGNAL_TYPE0	(4, (empty_t,		empty,		EmptySignal,	void*))
#define SIGNAL_TYPE1	(4, (integer_t,		integer,	IntegerSignal,	boost::int32_t))
#define SIGNAL_TYPE2	(4, (real_t,		real,		RealSignal,	double))
#define SIGNAL_TYPE3	(4, (complex_t,		complex,	ComplexSignal,	std::complex< double >))
#define SIGNAL_TYPE4	(4, (string_t,		string,		StringSignal,	std::string))
#define SIGNAL_TYPE5	(4, (logical_t,		logical,	BitSignal,	bool))
#define SIGNAL_TYPE6	(4, (uint8_t,		uint8,		UByteSignal,	boost::uint8_t))
#define SIGNAL_TYPE7	(4, (uint16_t,		uint16,		UWordSignal,	boost::uint16_t))
#define SIGNAL_TYPE8	(4, (uint32_t,		uint32,		UQuadSignal,	boost::uint32_t))
#define SIGNAL_TYPE9	(4, (int8_t,		int8,		ByteSignal,	boost::int8_t))
#define SIGNAL_TYPE10	(4, (int16_t,		int16,		WordSignal,	boost::int16_t))
#define SIGNAL_TYPE11	(4, (int32_t,		int32,		QuadSignal,	boost::int32_t))

#define SIGNAL_TYPE_CNT 12


typedef enum
{
#define BOOST_PP_DEF(z, I, _) \
	BOOST_PP_ARRAY_ELEM(1, SIGNAL_TYPE(I)) BOOST_PP_IF(BOOST_PP_EQUAL(I, 0), = I, BOOST_PP_EMPTY()) \
	BOOST_PP_COMMA_IF(BOOST_PP_LESS(I, BOOST_PP_SUB(SIGNAL_TYPE_CNT, 1)))

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF
} type_t;



#define BOOST_PP_DEF(z, I, _) /* this macro creates all necessary typedefs */ \
	typedef BOOST_PP_ARRAY_ELEM(3, SIGNAL_TYPE(I)) BOOST_PP_ARRAY_ELEM(0, SIGNAL_TYPE(I));

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF


#define vector_t std::vector

typedef vector_t< empty_t > empty_vec_t;
typedef vector_t< real_t > real_vec_t;
typedef vector_t< integer_t > int_vec_t;
typedef vector_t< complex_t > complex_vec_t;
typedef vector_t< logical_t > bit_vec_t;
typedef vector_t< string_t > string_vec_t;

#undef vector_t

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

// these assertions make sure the Variable::save_type_change methods works as expected
namespace type_ordering
{
	BOOST_STATIC_ASSERT(complex > real);
	BOOST_STATIC_ASSERT(real > integer);
	BOOST_STATIC_ASSERT(integer > empty);
}

#endif // _TYPES_HPP
