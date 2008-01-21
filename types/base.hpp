#ifndef TYPES_HPP
#define TYPES_HPP

#include <complex>
#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/comparison/less.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/empty.hpp>



#define SIGNAL_TYPE(I) BOOST_PP_CAT(SIGNAL_TYPE, I)

#define CPP_TYPE(I)   BOOST_PP_ARRAY_ELEM(0, SIGNAL_TYPE(I))
#define TYPE_VALUE(I) BOOST_PP_ARRAY_ELEM(1, SIGNAL_TYPE(I))
#define SIG_TYPE(I)   BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I))
#define ORIG_TYPE(I)  BOOST_PP_ARRAY_ELEM(3, SIGNAL_TYPE(I))

//			name of C++ type	name in type_t	signal name	typedef'ed from
#define SIGNAL_TYPE0	(4, (empty_t,		empty,		EmptySignal,	void*))
#define SIGNAL_TYPE1	(4, (logical_t,		logical,	BitSignal,	bool))
#define SIGNAL_TYPE2	(4, (uint8_t,		uint8,		UByteSignal,	boost::uint8_t))
#define SIGNAL_TYPE3	(4, (uint16_t,		uint16,		UWordSignal,	boost::uint16_t))
#define SIGNAL_TYPE4	(4, (uint32_t,		uint32,		UQuadSignal,	boost::uint32_t))
#define SIGNAL_TYPE5	(4, (int8_t,		int8,		ByteSignal,	boost::int8_t))
#define SIGNAL_TYPE6	(4, (int16_t,		int16,		WordSignal,	boost::int16_t))
#define SIGNAL_TYPE7	(4, (int32_t,		int32,		QuadSignal,	boost::int32_t))
#define SIGNAL_TYPE8	(4, (real_t,		real,		RealSignal,	double))
#define SIGNAL_TYPE9	(4, (complex_t,		complex,	ComplexSignal,	std::complex< double >))
#define SIGNAL_TYPE10	(4, (string_t,		string,		StringSignal,	std::string))

#define SIGNAL_TYPE_CNT 11


// pack all names in the "name in type_t" column into an enumeration
typedef enum
{
#define BOOST_PP_DEF(z, I, _) \
	TYPE_VALUE(I) BOOST_PP_IF(BOOST_PP_EQUAL(I, 0), = I, BOOST_PP_EMPTY()) \
	BOOST_PP_COMMA_IF(BOOST_PP_LESS(I, BOOST_PP_SUB(SIGNAL_TYPE_CNT, 1)))

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF
} type_t;



// creates all necessary typedefs from the "name of C++ type" and "typedef'ed from" columns
#define BOOST_PP_DEF(z, I, _) \
	typedef ORIG_TYPE(I) CPP_TYPE(I);

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF



// these assertions make sure the Variable::save_type_change methods works as expected
namespace type_ordering
{
	BOOST_STATIC_ASSERT(complex > real);
	BOOST_STATIC_ASSERT(real > int32);
	BOOST_STATIC_ASSERT(int32 > empty);
}

#endif // TYPES_HPP
