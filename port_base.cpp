#include "port_base.hpp"
#include "exceptions.hpp"



BasePort::BasePort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size) :
	get_buffer_ptr(0),
	name_(name),
	owner_block_name_(),
	type_(type),
	Ts_(Ts),
	frame_size_(frame_size),
	signal_buffer_id_(0)
{

}



const std::string& BasePort::get_name() const
{
	return name_;
}



const std::string& BasePort::get_owner_block_name() const
{
	return owner_block_name_;
}



void BasePort::set_owner_block_name(const string_t& owner_name)
{
	owner_block_name_ = owner_name;
}



real_t BasePort::get_Ts() const
{
	return Ts_;
}



bool BasePort::operator==(const BasePort& other) const
{
	return (this->get_name() == other.get_name());
}



uint32_t BasePort::get_frame_size() const
{
	return frame_size_;
}



type_t BasePort::get_type() const
{
	return type_;
}



uint32_t BasePort::get_signal_buffer_id() const
{
	return signal_buffer_id_;
}



BasePort::~BasePort()
{

}



void BasePort::set_type(type_t t)
{
	type_ = t;
}



void BasePort::set_Ts(real_t Ts)
{
	Ts_ = Ts;
}



void BasePort::set_frame_size(uint32_t framesize)
{
	frame_size_ = framesize;
}
