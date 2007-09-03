#ifndef _SIGNAL_HPP
#define _SIGNAL_HPP

#include "types.hpp"

#include <boost/ptr_container/ptr_vector.hpp>
#include <vector>
#include <algorithm>


class Signal
{
public:
	virtual ~Signal() {};

	typedef boost::ptr_vector< Signal > store_t;

protected:
	Signal() { }

	std::vector< uint8_t > dimensions_;

	real_t Ts_;
};


template< class T >
class SignalStore
{
public:
	typedef T element_t;

	SignalStore(uint32_t size)
	{
		data_array_ = new T[size];
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
};


class IntegerSignal : public Signal, public SignalStore< integer_t >
{
public:
	IntegerSignal(uint32_t size) : SignalStore< integer_t >(size) {};

	virtual ~IntegerSignal() {};
};


class RealSignal : public Signal, public SignalStore< real_t >
{
public:
	RealSignal(uint32_t size) : SignalStore< real_t >(size) {};

	virtual ~RealSignal() {};
};


class ComplexSignal : public Signal, public SignalStore< complex_t >
{
public:
	ComplexSignal(uint32_t size) : SignalStore< complex_t >(size) {};

	virtual ~ComplexSignal() {};
};


class StringSignal : public Signal, public SignalStore< string_t >
{
public:
	StringSignal(uint32_t size) : SignalStore< string_t >(size) {};

	virtual ~StringSignal() {};
};


class BitSignal : public Signal, public SignalStore< logical_t >
{
public:
	BitSignal(uint32_t size) : SignalStore< logical_t >(size) {};

	virtual ~BitSignal() {};
};

#endif // _SIGNAL_HPP
