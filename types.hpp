#ifndef _TYPES_HPP
#define _TYPES_HPP

#include <stdint.h>
#include <complex>
#include <vector>

/** \file types.hpp

   \brief A documented file
*/

typedef enum
{
	empty = 0,
	integer,
	real,
	complex,
	string,
	logical
} type_t;

/** \var typedef int32_t integer_t
 *  \brief The type for all integer-valued variables
 */
typedef int32_t integer_t;

/** \var typedef double real_t
 *  \brief The type for all real-valued variables
 */
typedef double real_t;

/** \var typedef std::complex<real_t> complex_t
 *  \brief The type for all complex-valued variables
 */
typedef std::complex< real_t > complex_t;

/** \var typedef std::string string_t
 *  \brief The type for all string variables
 */
typedef std::string string_t;

/** \var typedef void* empty_t
 *  \brief The type for all undecided variables
 */

typedef bool logical_t;

typedef void* empty_t;

typedef std::vector< empty_t > empty_vec_t;

typedef std::vector< real_t > real_vec_t;

typedef std::vector< integer_t > int_vec_t;

typedef std::vector< complex_t > complex_vec_t;

typedef std::vector< logical_t > bit_vec_t;

typedef std::vector< string_t > string_vec_t;

#endif // _TYPES_HPP
