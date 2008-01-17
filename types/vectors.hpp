#ifndef _VECTORS_HPP
#define _VECTORS_HPP

#define vector_t std::vector

typedef vector_t< empty_t > empty_vec_t;
typedef vector_t< real_t > real_vec_t;
typedef vector_t< integer_t > int_vec_t;
typedef vector_t< complex_t > complex_vec_t;
typedef vector_t< logical_t > bit_vec_t;
typedef vector_t< string_t > string_vec_t;

#undef vector_t

#endif // _VECTORS_HPP
