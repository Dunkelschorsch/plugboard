#ifndef _SIGNAL_HPP
#define _SIGNAL_HPP

#include "types.hpp"

#include <algorithm>



class Signal
{
public:
	virtual ~Signal() {};

	typedef std::vector< Signal* > store_t;

protected:
	Signal() : signal_type_(empty), Ts_(0), dimensions_() { }

	type_t signal_type_;

	std::vector< uint8_t > dimensions_;

	real_t Ts_;
};



template< class ElementT >
class SignalStore
{
public:
	typedef ElementT element_t;

	SignalStore(uint32_t size) : size_(size), data_array_()
	{
		data_array_ = new element_t[size_];
	}
	
	SignalStore(SignalStore &other)
	{
		size_ = other.size_;
		std::copy(other.data_array_, other.data_array_ + other.size_, data_array_);
	}

	/* maybe that's wrong, but it's never used anyway */
	SignalStore& operator=(const SignalStore& other)
	{
		return SignalStore(other);
	}

	~SignalStore()
	{
		delete[] data_array_;
	}

	element_t * get_data() const
	{
		return data_array_;
	}

	void put(std::vector< element_t >& v)
	{
		copy(v.begin(), v.end(), data_array_);
	}

	element_t* data_array_;
	uint32_t size_;
};



// this macro creates all necessary Signal-derived classes
#define BOOST_PP_DEF(z, I, _)  \
	class BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I)) :			\
		public Signal, public SignalStore< BOOST_PP_ARRAY_ELEM(0, SIGNAL_TYPE(I)) > \
	{								\
		public:							\
									\
		BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I))(uint32_t size) :	\
			SignalStore< BOOST_PP_ARRAY_ELEM(0, SIGNAL_TYPE(I)) >(size)	\
		{							\
 			signal_type_ = BOOST_PP_ARRAY_ELEM(1, SIGNAL_TYPE(I));		\
		};							\
		~BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I))() {};		\
		typedef BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I))* result_type;		\
	};

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _);
#undef BOOST_PP_DEF

#endif // _SIGNAL_HPP
