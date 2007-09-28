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
	Signal() : Ts_(0), dimensions_() { }

	std::vector< uint8_t > dimensions_;

	real_t Ts_;
};



template< class T >
class SignalStore
{
public:
	typedef T element_t;

	SignalStore(uint32_t size) : size_(size), data_array_()
	{
		data_array_ = new T[size_];
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

	element_t * get_data()
	{
		return data_array_;
	}

	void put(std::vector< element_t >* v)
	{
		copy(v->begin(), v->end(), data_array_);
	}

	element_t* data_array_;
	uint32_t size_;
};



#define BOOST_PP_DEF(z, I, _) /* this macro creates all necessary Signal-derived classes */ \
	class BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I)) :			\
		public Signal, public SignalStore< BOOST_PP_ARRAY_ELEM(0, SIGNAL_TYPE(I)) > \
	{								\
		public:							\
		BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I))(uint32_t size) :	\
			SignalStore< BOOST_PP_ARRAY_ELEM(0, SIGNAL_TYPE(I)) >(size) {};	\
		~BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I))() {};	\
	};

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)
#undef BOOST_PP_DEF

#endif // _SIGNAL_HPP
