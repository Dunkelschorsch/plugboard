#ifndef _PORT_OUT_HPP
#define _PORT_OUT_HPP

#include "port_base.hpp"
#include <deque>
#include "message.hpp"
class InPort;


class OutPort : public BasePort< OutPort >
{
public:
	OutPort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size);

	void connect(InPort& other, uint32_t signal_buffer_id);

	void disconnect(const InPort& other);

	boost::function< Message() > send;

	typedef std::deque< OutPort > store_t;
};

#endif // _PORT_OUT_HPP
