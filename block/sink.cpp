#include "sink.hpp"


Sink::Sink() :
	num_input_ports_(0),
	ports_in_()
{ }


Sink::~Sink() { }


uint16_t Sink::get_num_input_ports() const
{
	return num_input_ports_;
}



InPort::store_t * Sink::get_port_list( )
{
	return &ports_in_;
}
