#ifndef PORT_IN_HPP
#define PORT_IN_HPP

#include "port/port_base.hpp"
#include "visibility.hpp"
class Message;


class DSOEXPORT InPort : public BasePort< InPort >
{
public:

	InPort(const string_t& name, const type_t type=empty, const real_t Ts=0, const uint32_t frame_size=0);

	const Message receive(uint32_t buff_num);
};


#endif // PORT_IN_HPP
