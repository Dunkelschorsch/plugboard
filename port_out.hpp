#ifndef _PORT_OUT_HPP
#define _PORT_OUT_HPP

#include "port_base.hpp"
#include <deque>
class Message;
class InPort;



class OutPort : public BasePort
{
public:
	OutPort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size);

	void connect(InPort& other, uint32_t signal_buffer_id);

	void disconnect(const InPort& other);

	boost::function< Message&() > send;

	~OutPort();

	typedef std::deque< OutPort > store_t;
};

#endif // _PORT_OUT_HPP
