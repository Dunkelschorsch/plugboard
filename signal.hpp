#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include "types/base.hpp"

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

	SignalStore(size_t size) : size_(size), data_array_()
	{
		data_array_ = new element_t[size_];
		std::fill
		(
			data_array_,
			data_array_ + size_,
			static_cast< element_t >(0)
		);
	}

	SignalStore(const SignalStore& other) : size_(other.size_)
	{
		this->data_array_ = new element_t[size_];
		std::copy
		(
			other.data_array_,
			other.data_array_ + other.size_,
			this->data_array_
		);
	}

	SignalStore& operator=(const SignalStore& other)
	{
		if(this != &other)
		{
			SignalStore tmp(other);
			swap(tmp);
		}
		return *this;
	}

	~SignalStore()
	{
		delete[] data_array_;
	}

	element_t * get_data() const
	{
		return data_array_;
	}

private:
	size_t size_;
	element_t* data_array_;

	void swap(SignalStore& other)
	{
		using std::swap;
		swap(this->size_, other.size_);
		swap(this->data_array_, other.data_array_);
	}
};



// this macro creates all necessary Signal-derived classes
#define BOOST_PP_DEF(z, I, _)  \
	class SIG_TYPE(I) : public Signal, public SignalStore< CPP_TYPE(I) > \
	{						\
		public:					\
		SIG_TYPE(I)(size_t size) : SignalStore< CPP_TYPE(I) >(size)\
		{					\
	 		signal_type_ = TYPE_VALUE(I);	\
		};					\
		typedef SIG_TYPE(I)* result_type;	\
	};

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)
#undef BOOST_PP_DEF

#endif // SIGNAL_HPP
