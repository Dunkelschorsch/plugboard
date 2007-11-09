#include "port_in.hpp"
#include "message.hpp"
#include <iostream>


InPort::InPort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size) :
	BasePort(name, type, Ts, frame_size)
{

}



const Message InPort::receive(uint32_t buff_num)
{
#ifndef NDEBUG
	std::cout << "Hurra!! Geht!!!! Portname: '" << get_name() << "' buff_num: " << buff_num << 
		" Ts: " << Ts_ << " framesize: " << frame_size_ << std::endl;
#endif
	return Message();
}



InPort::~InPort()
{

}
