#ifndef PORT_BASE_HPP
#define PORT_BASE_HPP

#include "types/base.hpp"
#include <tr1/functional>
#include <list>
#include <algorithm>


template< class DerivedPortT >
class BasePort
{
friend class SystemImpl;

public:
	typedef std::list< DerivedPortT > store_t;
	
	const std::string& get_name() const;
	const std::string& get_owner_block_name() const;

	type_t get_type() const;
	real_t get_Ts() const;
	uint32_t get_frame_size() const;

	void set_owner_block_name (const std::string&);

	void set_type(type_t);
	void set_Ts(real_t);
	void set_frame_size(uint32_t);

	uint32_t get_signal_buffer_id() const;

	bool operator==(const BasePort< DerivedPortT >&) const;

	inline void* get_signal_ptr() const
	{
		return buffer_access_();
	}

protected:
	BasePort(const std::string&, const type_t, const real_t, const uint32_t);

	uint32_t signal_buffer_id_;

	std::tr1::function< void*() > buffer_access_;

private:
	void swap(BasePort&);

	std::string name_;

	std::string owner_block_name_;

	type_t type_;

	real_t Ts_;

	uint32_t frame_size_;


};



template< class DerivedPortT >
BasePort< DerivedPortT >::BasePort(const std::string& name, const type_t type, const real_t Ts, const uint32_t frame_size) :
	signal_buffer_id_(0),
	buffer_access_(0),
	name_(name),
	owner_block_name_(),
	type_(type),
	Ts_(Ts),
	frame_size_(frame_size)
{
}


template< class DerivedPortT >
void BasePort< DerivedPortT >::swap(BasePort& other)
{
	using std::swap;

	swap(this->name_, other.name_);
	swap(this->owner_block_name_, other.owner_block_name_);
	swap(this->type_, other.type_);
	swap(this->Ts_, other.Ts_);
	swap(this->frame_size_, other.frame_size_);
	swap(this->signal_buffer_id_, other.signal_buffer_id_);
	swap(this->buffer_access_, other.buffer_access_);
}
// hmm... wrote a swap function but no copy constructor or assignement operator?! interesting...

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
void BasePort< DerivedPortT >::set_owner_block_name(const std::string& owner_name)
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
void BasePort< DerivedPortT >::set_frame_size(uint32_t frame_size)
{
	frame_size_ = frame_size;
}

#endif // PORT_BASE_HPP
