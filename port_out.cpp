#include "port_out.hpp"
#include "port_in.hpp"
#include "exceptions.hpp"
#include <boost/bind.hpp>



OutPort::OutPort(const string_t& name, const type_t type, const real_t Ts, const uint32_t frame_size) :
	BasePort(name, type, Ts, frame_size),
	send(0)
{

}



void OutPort::connect(InPort & other, uint32_t signal_buffer_id)
{
	if (other.Ts_ > 0 && this->Ts_ != other.Ts_)
		throw SampleTimesMismatchException(other.get_owner_block_name()+"::"+other.get_name());

	if (other.type_ != empty && this->type_ != other.type_)
		throw SignalTypesMismatchException(other.get_owner_block_name()+"::"+other.get_name());

	if (other.frame_size_ > 0 && this->frame_size_ != other.frame_size_)
		throw FrameSizesMismatchException(other.get_owner_block_name()+"::"+other.get_name());
	
	other.Ts_ = this->Ts_;
	other.type_ = this->type_;
	other.frame_size_ = this->frame_size_;

	signal_buffer_id_ = signal_buffer_id;

	send = boost::bind(&InPort::receive, boost::ref(other), signal_buffer_id);
}



void OutPort::disconnect(const InPort & other)
{
	send = 0;
	get_buffer_ptr = 0;
}



OutPort::~OutPort()
{

}
