#ifndef _PORT_BASE_HPP
#define _PORT_BASE_HPP

#include "types.hpp"
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

	string_t get_owner_block_name() const;

	void set_owner_block_name (const string_t& owner_name);

	boost::function< void*() > get_buffer_ptr;

protected:
	BasePort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size);

	string_t name_;

	string_t owner_block_name_;

	type_t type_;

	real_t Ts_;

	uint32_t frame_size_;

	uint32_t signal_buffer_id_;

	virtual ~BasePort();
};

#endif // _PORT_BASE_HPP
