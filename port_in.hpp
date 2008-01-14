#ifndef _PORT_IN_HPP
#define _PORT_IN_HPP

#include "port_base.hpp"
class Message;


class InPort : public BasePort< InPort >
{
public:

	InPort(const string_t& name, const type_t type=empty, const real_t Ts=0, const uint32_t frame_size=0);

	const Message receive(uint32_t buff_num);
};


#endif // _PORT_IN_HPP
