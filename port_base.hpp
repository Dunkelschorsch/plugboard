#ifndef _PORT_BASE_HPP
#define _PORT_BASE_HPP

#include "types.hpp"
#include <boost/function.hpp>

template< class DerivedPortT >
class BasePort
{
public:
	const std::string& get_name() const;

	type_t get_type() const;

	void set_type(type_t t);

	real_t get_Ts() const;

	void set_Ts(real_t Ts);

	uint32_t get_frame_size() const;

	void set_frame_size(uint32_t framesize);

	uint32_t get_signal_buffer_id() const;

	bool operator==(const BasePort< DerivedPortT >& other) const;

	const std::string& get_owner_block_name() const;

	void set_owner_block_name (const string_t& owner_name);

	boost::function< void*() > get_buffer_ptr;

protected:
	BasePort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size);

	string_t name_;

	string_t owner_block_name_;

	type_t type_;

	real_t Ts_;

	uint32_t frame_size_;

	uint32_t signal_buffer_id_;
};



template< class DerivedPortT >
BasePort< DerivedPortT >::BasePort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size) :
	get_buffer_ptr(0),
	name_(name),
	owner_block_name_(),
	type_(type),
	Ts_(Ts),
	frame_size_(frame_size),
	signal_buffer_id_(0)
{

}


template< class DerivedPortT >
const std::string& BasePort< DerivedPortT >::get_name() const
{
	return name_;
}


template< class DerivedPortT >
const std::string& BasePort< DerivedPortT >::get_owner_block_name() const
{
	return owner_block_name_;
}


template< class DerivedPortT >
void BasePort< DerivedPortT >::set_owner_block_name(const string_t& owner_name)
{
	owner_block_name_ = owner_name;
}


template< class DerivedPortT >
real_t BasePort< DerivedPortT >::get_Ts() const
{
	return Ts_;
}


template< class DerivedPortT >
bool BasePort< DerivedPortT >::operator==(const BasePort< DerivedPortT >& other) const
{
	return (this->get_name() == other.get_name());
}


template< class DerivedPortT >
uint32_t BasePort< DerivedPortT >::get_frame_size() const
{
	return frame_size_;
}


template< class DerivedPortT >
type_t BasePort< DerivedPortT >::get_type() const
{
	return type_;
}


template< class DerivedPortT >
uint32_t BasePort< DerivedPortT >::get_signal_buffer_id() const
{
	return signal_buffer_id_;
}



template< class DerivedPortT >
void BasePort< DerivedPortT >::set_type(type_t t)
{
	type_ = t;
}


template< class DerivedPortT >
void BasePort< DerivedPortT >::set_Ts(real_t Ts)
{
	Ts_ = Ts;
}


template< class DerivedPortT >
void BasePort< DerivedPortT >::set_frame_size(uint32_t framesize)
{
	frame_size_ = framesize;
}

#endif // _PORT_BASE_HPP
