#ifndef BLOCK_SINK_HPP
#define BLOCK_SINK_HPP

#include "port/port_in.hpp"

template< class PortT > class PortTraits;


class Sink
{
friend class PortTraits< InPort >;

public:
	Sink();
	virtual ~Sink();

	virtual bool setup_input_ports() = 0;
	uint16_t get_num_input_ports() const;

	InPort::store_t * get_port_list();
	InPort::store_t * get_port_list() const;

protected:
	uint16_t num_input_ports_;
	InPort::store_t ports_in_;
};


#endif // BLOCK_SINK_HPP
