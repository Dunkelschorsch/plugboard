#include "port.hpp"
#include <iostream>


BasePort::BasePort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size) :
	name_(name), type_(type), Ts_(Ts), frame_size_(frame_size)
{

}


string_t BasePort::get_name() const
{
	return name_;
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



OutPort::OutPort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size) :
	BasePort(name, type, Ts, frame_size)
{

}


void OutPort::connect(InPort & other, uint32_t signal_buffer_id)
{
	if (other.Ts_ > 0 && this->Ts_ != other.Ts_)
	{
		std::cout << "Sample times of ports do not match!" << std::endl;
	} else
	{
		other.Ts_ = this->Ts_;
	}

	if (other.type_ != empty && this->type_ != other.type_)
	{
		std::cout << "Signal types of ports do not match!" << std::endl;
	} else
	{
		other.type_ = this->type_;
	}

	if (other.frame_size_ > 0 && this->frame_size_ != other.frame_size_)
	{
		std::cout << "Frame sizes of ports do not match!" << std::endl;
	} else
	{
		other.frame_size_ = this->frame_size_;
	}

	signal_buffer_id_ = signal_buffer_id;

	send = boost::bind(&InPort::receive, boost::ref(other), signal_buffer_id);
}


void OutPort::disconnect(const InPort & other)
{
	send = 0;
	get_buffer_ptr = 0;
}


OutPort::~OutPort()
{

}


InPort::InPort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size) :
	BasePort(name, type, Ts, frame_size)
{

}


Message & InPort::receive(uint32_t buff_num)
{
	std::cout << "Hurra!! Geht!!!! Portname: '" << get_name() << "' buff_num: " << buff_num << 
		" Ts: " << Ts_ << " framesize: " << frame_size_ << std::endl;
}


InPort::~InPort()
{

}
