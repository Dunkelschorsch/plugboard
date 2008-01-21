#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include "types/base.hpp"
#include "types/typeinfo.hpp"

#include <vector>
#include <algorithm>



class Signal
{
public:
	virtual ~Signal() {};

	typedef std::vector< Signal* > store_t;

protected:
	Signal() : signal_type_(empty), dimensions_(), Ts_(0) { }

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

	uint32_t size_;
	element_t* data_array_;
};



// this macro creates all necessary Signal-derived classes
#define BOOST_PP_DEF(z, I, _)  \
	class SIG_TYPE(I) : public Signal, public SignalStore< CPP_TYPE(I) > \
	{						\
		public:					\
		SIG_TYPE(I)(uint32_t size) : SignalStore< CPP_TYPE(I) >(size)\
		{					\
	 		signal_type_ = TYPE_VALUE(I);	\
		};					\
		typedef SIG_TYPE(I)* result_type;	\
	};

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)
#undef BOOST_PP_DEF

#endif // SIGNAL_HPP
