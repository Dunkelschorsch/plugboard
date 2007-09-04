#include "system.hpp"
#include "exceptions.hpp"
#include "signal.hpp"
#include "port.hpp"
#include "symtab.hpp"
#include "factory.hpp"

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <iostream>


using boost::bind;
using boost::function;
using boost::lambda::new_ptr;



struct SystemImpl
{
	SystemImpl();

	inline uint32_t create_signal_buffer(type_t type, uint32_t size);

	inline void register_basic_types();

	inline void set_buffer_ptrs(OutPort& out, InPort& in, Signal* s);

	Block::store_t blocks_;

	Signal::store_t signal_buffers_;

	uint32_t signal_buffer_count_;

	typedef Factory< void*, type_t, function< Signal*(size_t) > > signal_factory_t;

	signal_factory_t signal_factory_;

	typedef Factory< void*, type_t, function< void*(Signal*) > > get_buffer_factory_t;

	get_buffer_factory_t get_buffer_factory_;

	double simulation_time;

	Symtab symtab_;
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
	
	uint32_t curr_sig_buffer =
		d->create_signal_buffer(source_port_it->get_type(), source_port_it->get_frame_size());

	d->set_buffer_ptrs(*source_port_it, *sink_port_it, &d->signal_buffers_[curr_sig_buffer]);
}



void System::wakeup_block(const std::string & name, uint32_t times=1)
{
	H_D(System)

	Block::store_t::iterator block_it =
		std::find_if(d->blocks_.begin(), d->blocks_.end(), bind(&Block::get_name_sys, _1) == name);

	for(uint32_t i=0; i<times; i++)
		block_it->wakeup();
}



namespace
{
	template< class T >
	void* get_buffer(Signal *s)
	{
		return dynamic_cast < T* >(s)->get_data();
	}
}



void SystemImpl::register_basic_types()
{
	/* IntegerSignal and integer_t */
	get_buffer_factory_.Register(integer, bind(&get_buffer< IntegerSignal >, _1));
	signal_factory_.Register(integer, bind< IntegerSignal* >(new_ptr< IntegerSignal >(), _1));

	/* RealSignal and real_t */
	get_buffer_factory_.Register(real, bind(&get_buffer< RealSignal >, _1));
	signal_factory_.Register(real, bind< RealSignal* >(new_ptr< RealSignal >(), _1));

	/* ComplexSignal and complex_t */
	get_buffer_factory_.Register(complex, bind(&get_buffer< ComplexSignal >, _1));
	signal_factory_.Register(complex, bind< ComplexSignal* >(new_ptr< ComplexSignal >(), _1));

	/* StringSignal and string_t */
	get_buffer_factory_.Register(string, bind(&get_buffer< StringSignal >, _1));
	signal_factory_.Register(string, bind< StringSignal* >(new_ptr< StringSignal >(), _1));

	/* BitSignal and logical_t */
	get_buffer_factory_.Register(logical, bind(&get_buffer< BitSignal >, _1));
	signal_factory_.Register(logical, bind< BitSignal* >(new_ptr< BitSignal >(), _1));
}



uint32_t SystemImpl::create_signal_buffer(type_t type, uint32_t size)
{
	signal_buffers_.push_back(signal_factory_.CreationFunction(type)(size));
	return signal_buffer_count_++;
}



void SystemImpl::set_buffer_ptrs(OutPort& out, InPort& in, Signal* s)
{
	function < void*(Signal*) > f;

	f = get_buffer_factory_.CreationFunction(out.get_type()) ;
	out.get_buffer_ptr = bind(f, s);
	in.get_buffer_ptr = bind(f, s);
}
