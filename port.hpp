#ifndef _PORT_HPP
#define _PORT_HPP

#include "types.hpp"
class Message;

#include <deque>
#include <boost/function.hpp>



class BasePort
{
friend class OutPort;
friend class InPort;

public:
	string_t get_name() const;

	type_t get_type() const;

	real_t get_Ts() const;

	uint32_t get_frame_size() const;

	uint32_t get_signal_buffer_id() const;

	bool operator==(const BasePort& other) const;

	boost::function< void*() > get_buffer_ptr;

protected:
	BasePort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size);

	string_t name_;

	type_t type_;

	real_t Ts_;

	uint32_t frame_size_;

	uint32_t signal_buffer_id_;

	virtual ~BasePort();
};


/* forward declaration neccessary for connect and disconnect methods */

class InPort;

class OutPort : public BasePort
{
public:
	OutPort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size);

	void connect(InPort& other, uint32_t signal_buffer_id);

	void disconnect(const InPort& other);

	boost::function< Message&() > send;

	virtual ~OutPort();

	typedef std::deque< OutPort > store_t;
};


class InPort : public BasePort
{
public:

	InPort(const string_t& name, const type_t type=empty, const real_t Ts=0, const uint32_t frame_size=0);

	Message& receive(uint32_t buff_num);

	virtual ~InPort();

	typedef std::deque< InPort > store_t;
};

#endif // _PORT_HPP
