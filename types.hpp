#ifndef _TYPES_HPP
#define _TYPES_HPP

#include <complex>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/array/elem.hpp>


/** \file types.hpp

   \brief A documented file
*/


#define SIGNAL_TYPE(I) SIGNAL_TYPE ## I

#define SIGNAL_TYPE0	(4, (empty_t,		empty,		EmptySignal,	void*))
#define SIGNAL_TYPE1	(4, (integer_t,		integer,	IntegerSignal,	uint32_t))
#define SIGNAL_TYPE2	(4, (real_t,		real,		RealSignal,	double))
#define SIGNAL_TYPE3	(4, (complex_t,		complex,	ComplexSignal,	std::complex< double >))
#define SIGNAL_TYPE4	(4, (string_t,		string,		StringSignal,	std::string))
#define SIGNAL_TYPE5	(4, (logical_t,		logical,	BitSignal, 	bool))

#define SIGNAL_TYPE_CNT 6


using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::int8_t;
using boost::int16_t;
using boost::int32_t;

typedef enum
{
	empty = 0,
	integer,
	real,
	complex,
	string,
	logical,
	/* atm unsused: */
	uint8,
	uint16,
	uint32,
	int8,
	int16,
	int32,
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

#endif // _TYPES_HPP
