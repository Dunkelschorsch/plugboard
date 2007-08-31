#include "system.hpp"

System::System()
{
	signal_buffer_count_ = 0;
	simulation_time = 0.0;
}


void System::add_block(Block* b, const std::string& name_sys)
{
	Block::store_t::const_iterator it =
		std::find_if(blocks_.begin(), blocks_.end(), boost::bind(&Block::get_name_sys, _1) == name_sys);

	if(it != blocks_.end())
	{
		delete b;
		throw duplicate_block_name_error(name_sys);
	}
	
	if (!b->is_configured())
	{
		/* maybe the block does not need to be configured */
		if (!(b->params_.size() == 0))
		{
			delete b;
			throw block_not_configured_error(name_sys);
		}
	}

	b->set_name_sys(name_sys);

	if(!b->setup_input_ports())
	{

	}

	if(!b->setup_output_ports())
	{

	}

	blocks_.push_back(b);
}


void System::connect_ports(const std::string & block_source,
			   const std::string & port_source,
			   const std::string & block_sink,
                           const std::string & port_sink)
{
	Block::store_t::iterator source_block_it, sink_block_it;

	OutPort::store_t::iterator source_port_it;
	InPort::store_t::iterator sink_port_it;

	/* check if source block given in "block_source" exists */
	source_block_it =
		std::find_if(blocks_.begin(), blocks_.end(), boost::bind(&Block::get_name_sys, _1) == block_source);

	if (source_block_it == blocks_.end())
	{
		throw non_existant_block_error(block_source);
	}

	/* check if source port given in "port_source" exists */
	source_port_it =
		std::find_if(source_block_it->ports_out_.begin(), source_block_it->ports_out_.end(), boost::bind(&OutPort::get_name, _1) == port_source);

	if (source_port_it == source_block_it->ports_out_.end())
	{
		throw non_existant_port_error(port_source);
	}

	/* check if sink block given in "block_sink" exists */
	sink_block_it =
		std::find_if(blocks_.begin(), blocks_.end(), boost::bind(&Block::get_name_sys, _1) == block_sink);

	if (sink_block_it == blocks_.end())
	{
		throw non_existant_block_error(block_sink);
	}

	/* check if sink port given in "port_sink" exists */
	sink_port_it =
		std::find_if(sink_block_it->ports_in_.begin(), sink_block_it->ports_in_.end(), boost::bind(&InPort::get_name, _1) == port_sink);

	if (sink_port_it == sink_block_it->ports_in_.end())
	{
		throw non_existant_port_error(port_sink);
	}

	if (source_port_it->send != 0)
	{
		std::cout << "already connected!" << std::endl;
		return;
	}

	/* make the send() method of the source port call the right method of the sink port */
	source_port_it->connect(*sink_port_it, signal_buffer_count_);
	
	uint32_t curr_sig_buffer = create_signal_buffer(source_port_it->get_type(), source_port_it->get_frame_size());
	
	switch (source_port_it->get_type())
	{
		case integer:
			source_port_it->get_buffer_ptr =
				boost::bind(&IntegerSignal::get_data,
				dynamic_cast < IntegerSignal* >(&signal_buffers_[curr_sig_buffer]));
			break;

		case real:
			source_port_it->get_buffer_ptr =
				boost::bind(&RealSignal::get_data,
				dynamic_cast < RealSignal* >(&signal_buffers_[curr_sig_buffer]));
			break;

		case complex:
			source_port_it->get_buffer_ptr =
				boost::bind(&ComplexSignal::get_data,
				dynamic_cast < ComplexSignal* >(&signal_buffers_[curr_sig_buffer]));

			sink_port_it->get_buffer_ptr =
				boost::bind(&ComplexSignal::get_data,
				dynamic_cast < ComplexSignal* >(&signal_buffers_[curr_sig_buffer]));
			break;

		case string:
			source_port_it->get_buffer_ptr =
				boost::bind(&StringSignal::get_data,
				dynamic_cast < StringSignal* >(&signal_buffers_[curr_sig_buffer]));
			break;

		case logical:
			source_port_it->get_buffer_ptr =
				boost::bind(&BitSignal::get_data,
				dynamic_cast < BitSignal* >(&signal_buffers_[curr_sig_buffer]));
			break;

		default:
			/* apparently something just went boink on us */
			std::cerr << "Trying to connect a port of unknown type!" << std::endl;
			break;
	}
	
}


uint32_t System::create_signal_buffer(type_t type, uint32_t size)
{
	switch (type)
	{
		case real:
			signal_buffers_.push_back(new RealSignal(size));
			break;
		case integer:
			signal_buffers_.push_back(new IntegerSignal(size));
			break;
		case complex:
			signal_buffers_.push_back(new ComplexSignal(size));
			break;
		case string:
			signal_buffers_.push_back(new StringSignal(size));
			break;
		default:
			/* apparently something just went boink on us */
			std::cerr << "Trying to connect a port of unknown type!" << std::endl;
			break;
	}
	
	return signal_buffer_count_++;
}
System::~ System()
{
	
}

void System::wakeup_block(const std::string & name, uint32_t times=1)
{
	Block::store_t::iterator block_it =
		std::find_if(blocks_.begin(), blocks_.end(), boost::bind(&Block::get_name_sys, _1) == name);

	for(uint32_t i=0; i<times; i++)
		block_it->wakeup();

}
