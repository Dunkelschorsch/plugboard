#include "source.hpp"


Source::Source() :
	connect_calls(),
	num_output_ports_(0),
	ports_out_(),
	connected_blocks_() { }


Source::~Source() { }


uint16_t Source::get_num_output_ports() const
{
	return num_output_ports_;
}


OutPort::store_t * Source::get_port_list( )
{
	return &ports_out_;
}


const std::set< std::string >& Source::get_connections() const
{
	return connected_blocks_;
}


void Source::add_connection(const std::string & name)
{
	connected_blocks_.insert(name);
}
