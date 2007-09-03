#include "system.hpp"
#include "exceptions.hpp"
#include "signal.hpp"
#include "port.hpp"
#include "symtab.hpp"
#include "factory.hpp"

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/casts.hpp>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <iostream>

using boost::bind;
/*
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/casts.hpp>
#include <boost/any.hpp>

#include <boost/statechart/event.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>

namespace sc = boost::statechart;
*/

struct SystemImpl
{
	SystemImpl();

	virtual ~SystemImpl() { };

	inline uint32_t create_signal_buffer(type_t type, uint32_t size);

	inline void register_basic_types();

	Block::store_t blocks_;

	Signal::store_t signal_buffers_;

	Symtab symtab_;

	uint32_t signal_buffer_count_;

	typedef Factory< void*, type_t, boost::function< Signal*(size_t) > > signal_factory_t;

	typedef Factory< void*, type_t, boost::function< void*() > > get_buffer_factory_t;

	get_buffer_factory_t get_buffer_factory_;

	signal_factory_t signal_factory_;

	double simulation_time;
};


SystemImpl::SystemImpl()
{
	signal_buffer_count_ = 0;
	simulation_time = 0.0;
	register_basic_types();
}


System::System() : d_ptr(new SystemImpl)
{

}


System::System(SystemImpl &dd) : d_ptr(&dd)
{

}


System::~System()
{
	delete d_ptr;
}


void SystemImpl::register_basic_types()
{
	signal_factory_.Register(integer, boost::bind< IntegerSignal* >(boost::lambda::new_ptr< IntegerSignal >(), _1));
	signal_factory_.Register(real, boost::bind< RealSignal* >(boost::lambda::new_ptr< RealSignal >(), _1));
	signal_factory_.Register(complex, boost::bind< ComplexSignal* >(boost::lambda::new_ptr< ComplexSignal >(), _1));
	signal_factory_.Register(string, boost::bind< StringSignal* >(boost::lambda::new_ptr< StringSignal >(), _1));
	signal_factory_.Register(logical, boost::bind< BitSignal* >(boost::lambda::new_ptr< BitSignal >(), _1));
}

void System::add_block(Block* b, const std::string& name_sys)
{
	H_D(System)

	Block::store_t::const_iterator it =
		std::find_if(d->blocks_.begin(), d->blocks_.end(), bind(&Block::get_name_sys, _1) == name_sys);

	if(it != d->blocks_.end())
	{
		delete b;
		throw duplicate_block_name_error(name_sys);
	}
	
	if (!b->is_configured())
	{
		/* maybe the block does not need to be configured */
		if (!(b->get_params().size() == 0))
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

	d->blocks_.push_back(b);
}


void System::connect_ports(const std::string & block_source,
			   const std::string & port_source,
			   const std::string & block_sink,
                           const std::string & port_sink)
{
	H_D(System)

	Block::store_t::iterator source_block_it, sink_block_it;

	OutPort::store_t::iterator source_port_it;
	InPort::store_t::iterator sink_port_it;

	/* check if source block given in "block_source" exists */
	source_block_it =
		std::find_if(d->blocks_.begin(), d->blocks_.end(), bind(&Block::get_name_sys, _1) == block_source);

	if (source_block_it == d->blocks_.end())
	{
		throw non_existant_block_error(block_source);
	}

	/* check if source port given in "port_source" exists */
	source_port_it =
		std::find_if(source_block_it->get_outport_list().begin(), source_block_it->get_outport_list().end(), bind(&OutPort::get_name, _1) == port_source);

	if (source_port_it == source_block_it->get_outport_list().end())
	{
		throw non_existant_port_error(port_source);
	}

	/* check if sink block given in "block_sink" exists */
	sink_block_it =
		std::find_if(d->blocks_.begin(), d->blocks_.end(), bind(&Block::get_name_sys, _1) == block_sink);

	if (sink_block_it == d->blocks_.end())
	{
		throw non_existant_block_error(block_sink);
	}

	/* check if sink port given in "port_sink" exists */
	sink_port_it =
		std::find_if(sink_block_it->get_inport_list().begin(), sink_block_it->get_inport_list().end(), bind(&InPort::get_name, _1) == port_sink);

	if (sink_port_it == sink_block_it->get_inport_list().end())
	{
		throw non_existant_port_error(port_sink);
	}

	if (source_port_it->send != 0)
	{
		std::cout << "already connected!" << std::endl;
		return;
	}

	/* make the send() method of the source port call the right method of the sink port */
	source_port_it->connect(*sink_port_it, d->signal_buffer_count_);
	
	uint32_t curr_sig_buffer = d->create_signal_buffer(source_port_it->get_type(), source_port_it->get_frame_size());
/*
	boost::function< void*() > rr;

	bind(&IntegerSignal::get_data,
		(boost::lambda::ll_dynamic_cast< IntegerSignal* >(_1) ) )(); //(&d->signal_buffers_[curr_sig_buffer]);
*/
	switch (source_port_it->get_type())
	{
		case integer:
			source_port_it->get_buffer_ptr =
				bind(&IntegerSignal::get_data,
				dynamic_cast < IntegerSignal* >(&d->signal_buffers_[curr_sig_buffer]));

			sink_port_it->get_buffer_ptr =
				bind(&IntegerSignal::get_data,
				dynamic_cast < IntegerSignal* >(&d->signal_buffers_[curr_sig_buffer]));
			break;

		case real:
			source_port_it->get_buffer_ptr =
				bind(&RealSignal::get_data,
				dynamic_cast < RealSignal* >(&d->signal_buffers_[curr_sig_buffer]));

			sink_port_it->get_buffer_ptr =
				bind(&RealSignal::get_data,
				dynamic_cast < RealSignal* >(&d->signal_buffers_[curr_sig_buffer]));
			break;

		case complex:
			source_port_it->get_buffer_ptr =
				bind(&ComplexSignal::get_data,
				dynamic_cast < ComplexSignal* >(&d->signal_buffers_[curr_sig_buffer]));

			sink_port_it->get_buffer_ptr =
				bind(&ComplexSignal::get_data,
				dynamic_cast < ComplexSignal* >(&d->signal_buffers_[curr_sig_buffer]));
			break;

		case string:
			source_port_it->get_buffer_ptr =
				bind(&StringSignal::get_data,
				dynamic_cast < StringSignal* >(&d->signal_buffers_[curr_sig_buffer]));

			sink_port_it->get_buffer_ptr =
				bind(&StringSignal::get_data,
				dynamic_cast < StringSignal* >(&d->signal_buffers_[curr_sig_buffer]));
			break;

		case logical:
			source_port_it->get_buffer_ptr =
				bind(&BitSignal::get_data,
				dynamic_cast < BitSignal* >(&d->signal_buffers_[curr_sig_buffer]));

			sink_port_it->get_buffer_ptr =
				bind(&BitSignal::get_data,
				dynamic_cast < BitSignal* >(&d->signal_buffers_[curr_sig_buffer]));
			break;

		default:
			/* apparently something just went boink on us */
			std::cerr << "Trying to connect a port of unknown type!" << std::endl;
			break;
	}
	
}


uint32_t SystemImpl::create_signal_buffer(type_t type, uint32_t size)
{
	
	signal_buffers_.push_back(signal_factory_.CreationFunction(type)(size));
	return signal_buffer_count_++;
}


void System::wakeup_block(const std::string & name, uint32_t times=1)
{
	H_D(System)

	Block::store_t::iterator block_it =
		std::find_if(d->blocks_.begin(), d->blocks_.end(), bind(&Block::get_name_sys, _1) == name);

	for(uint32_t i=0; i<times; i++)
		block_it->wakeup();

}
