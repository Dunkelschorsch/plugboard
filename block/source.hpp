#ifndef BLOCK_SOURCE_HPP
#define BLOCK_SOURCE_HPP

#include <vector>
#include <set>
#include "port/port_out.hpp"
#include "port/port_in.hpp"

template< class PortT > class PortTraits;


class Source
{
friend class PortTraits< OutPort >;

public:
	Source();
	virtual ~Source();

	virtual bool setup_output_ports() = 0;
	uint16_t get_num_output_ports() const;

	OutPort::store_t * get_port_list();
	OutPort::store_t * get_port_list() const;
	const std::set< std::string >& get_connections() const;
	void add_connection(const std::string& name);

	std::vector< std::pair< OutPort::store_t::iterator, InPort::store_t::iterator > > connect_calls;

protected:
	uint16_t num_output_ports_;
	OutPort::store_t ports_out_;
	std::set< std::string > connected_blocks_;
};


#endif // BLOCK_SOURCE_HPP
