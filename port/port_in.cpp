#include "port/port_in.hpp"
#include "message.hpp"

#ifndef NDEBUG
#include <iostream>
#endif


namespace plugboard
{
	InPort::InPort(const std::string& name, const type_t type, const real_t Ts, const uint32_t frame_size) :
		BasePort< InPort >(name, type, Ts, frame_size)
	{

	}



	const Message InPort::receive(uint32_t buff_num)
	{
#ifndef NDEBUG
		std::cout << "Hurra!! Geht!!!! Portname: '" << get_name() << "' buff_num: " << buff_num << 
			" Ts: " << get_Ts() << " framesize: " << get_frame_size() << std::endl;
#endif
		return Message();
	}
} // namespace plugboard
