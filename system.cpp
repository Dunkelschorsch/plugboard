#include "system.hpp"
#include "exceptions.hpp"
#include "signal.hpp"
#include "block.hpp"
#include "variable.hpp"
#include "port.hpp"
#include "symtab.hpp"
#include "factory.hpp"

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <iostream>


using boost::bind;
using boost::function;
using boost::lambda::new_ptr;
using boost::lambda::delete_ptr;


struct SystemImpl
{
	SystemImpl();

	~SystemImpl();

	inline uint32_t create_signal_buffer(type_t type, uint32_t size);

	inline void register_basic_types();

	inline void set_buffer_ptrs(OutPort& out, InPort& in, Signal *s);

	void add_block_impl(Block *b, const std::string& name_sys);

	Signal::store_t signal_buffers_;

	uint32_t signal_buffer_count_;

	typedef Factory< void*, type_t, function< Signal*(size_t) > > signal_factory_t;

	signal_factory_t signal_factory_;

	typedef Factory< void*, type_t, void* (*)(Signal*) > get_buffer_factory_t;

	get_buffer_factory_t get_buffer_factory_;

	double simulation_time;

	Symtab symtab_;

	typedef std::vector< Block::store_t > block_list_t;

	block_list_t blocks_;

	std::vector< std::string > block_names_;
};



SystemImpl::SystemImpl()
{
	signal_buffer_count_ = 0;
	simulation_time = 0.0;

	register_basic_types();
}



SystemImpl::~SystemImpl()
{
	for
	(
		block_list_t::iterator it = blocks_.begin();
		it != blocks_.end();
		it++
	)
	std::for_each
	(
		it->begin(),
		it->end(),
		boost::lambda::bind(delete_ptr(), boost::lambda::_1)
	);
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



void System::add_block(Block *b, const std::string& name_sys)
{
	H_D(System)
	d->add_block_impl(b, name_sys);
}



void SystemImpl::add_block_impl(Block *b, const std::string& name_sys)
{
	/* determine if the given block name already exists */
	std::vector< std::string >::const_iterator it =
		std::find_if
		(
			block_names_.begin(),
			block_names_.end(),
			boost::lambda::_1 == name_sys
		);

	if(it != block_names_.end())
	{
		delete b;
		throw duplicate_block_name_error(name_sys);
	}

	/* if we make it here, we can set up and add the blocks to the system */
	if (!b->is_configured())
	{
		/* maybe the block does not need to be configured */
		if (!(b->get_params().size() == 0))
		{
			delete b;
			throw block_not_configured_error(name_sys);
		}
	}

	/* give it its unique name */
	b->set_name_sys(name_sys);

	if(!b->setup_input_ports())
	{

	}

	if(!b->setup_output_ports())
	{

	}

	/* create a new vector that contains the Block-pointer b as its only element 
	   and append it to the block list */
	blocks_.push_back(Block::store_t(1, b));

#ifndef NDEBUG
	std::cerr << blocks_.size()-1 << ": "
		<< blocks_.back().front()->get_name_sys() <<  " type: "
		<< blocks_.back().front()->get_name() << std::endl;
#endif

	/* add the block's name to prevent further usage */
	block_names_.push_back(name_sys);
}



void System::connect_ports(const std::string & block_source,
			   const std::string & port_source,
			   const std::string & block_sink,
                           const std::string & port_sink)
{
	H_D(System)

#ifndef NDEBUG
	std::cerr << "connect " << block_source << " --> " << block_sink << std::endl;
#endif

	int32_t source_block_list_num = -1,
		sink_block_list_num = -1;

	OutPort::store_t::iterator source_port_it;
	InPort::store_t::iterator sink_port_it;

	Block::store_t::iterator source_block_pos;
	Block::store_t::iterator sink_block_pos;

	/* no, i will NOT try to replace this with a single standard algorithm invocation ... */
	for(uint32_t i=0; i<d->blocks_.size(); i++)
	{
		/* check if source block given in "block_source" exists */
		Block::store_t::iterator source_block_it =
			std::find_if
			(
				d->blocks_[i].begin(),
				d->blocks_[i].end(),
				bind(&Block::get_name_sys, _1) == block_source
			);

		if(source_block_it != d->blocks_[i].end())
 		{
			/* so we found our block. now let's see if the given port name was valid */
			source_port_it =
				std::find_if
				(
					(*source_block_it)->get_outport_list().begin(),
					(*source_block_it)->get_outport_list().end(),
					bind(&OutPort::get_name, _1) == port_source
				);

			/* unfortunately the given port name was invalid */
			if (source_port_it == (*source_block_it)->get_outport_list().end())
			{
				throw non_existant_port_error(port_source);
			}

			if (source_port_it->send != 0)
			{
				/* the ports exists, but has already been connected */
				std::cerr << "already connected!" << std::endl;
				return;
			}
			/* the ports exists. all is fine. remember where the block is stored */
			source_block_list_num = i;
			source_block_pos = source_block_it;
		} 

		/* check if sink block given in "block_sink" exists */
		Block::store_t::iterator sink_block_it =
			std::find_if
			(
				d->blocks_[i].begin(),
				d->blocks_[i].end(),
				bind(&Block::get_name_sys, _1) == block_sink
			);
		
		if(sink_block_it != d->blocks_[i].end())
		{
			/* so we found our block. now let's see if the given port name was valid */
			sink_port_it =
				std::find_if
				(
					(*sink_block_it)->get_inport_list().begin(),
					(*sink_block_it)->get_inport_list().end(),
					bind(&InPort::get_name, _1) == port_sink
				);

			/* unfortunately the given port name was invalid */
			if (sink_port_it == (*sink_block_it)->get_inport_list().end())
			{
				throw non_existant_port_error(port_sink);
			}

			/* the ports exists. all is fine. remember where the block is stored */
			sink_block_list_num = i;
			sink_block_pos = sink_block_it;
		}
	} // <- that is from a for-loop

	if (source_block_list_num == -1)
	{
		/* apparently the source block does not exist */
		throw non_existant_block_error(block_source);
	}

	if (sink_block_list_num == -1)
	{
		/* apparently the sink block does not exist */
		throw non_existant_block_error(block_sink);
	}


#ifndef NDEBUG
	std::cerr << "found source block in vector no. " << source_block_list_num << std::endl;
	std::cerr << "found sink block in vector no. " << sink_block_list_num << std::endl;
#endif

	if (sink_block_list_num != source_block_list_num)
	{
		if ((d->blocks_[source_block_list_num].back())->get_num_output_ports() > 1)
		{
			d->blocks_[source_block_list_num].insert
			(
				source_block_pos+1,
				d->blocks_[sink_block_list_num].begin(),
				d->blocks_[sink_block_list_num].end()
			);
#ifndef NDEBUG
			std::cerr << "deleting: " << std::endl;
			std::for_each
			(
				d->blocks_.at(sink_block_list_num).begin(),
				d->blocks_.at(sink_block_list_num).end(),
				std::cerr << boost::lambda::bind(&Block::get_name_sys, boost::lambda::_1) << " "
			);
			std::cerr << std::endl;
#endif
			d->blocks_.erase(d->blocks_.begin() + sink_block_list_num);
		} else
		{
			d->blocks_[sink_block_list_num].insert
			(
				sink_block_pos,
				d->blocks_[source_block_list_num].begin(),
				d->blocks_[source_block_list_num].end()
			);
#ifndef NDEBUG
			std::cerr << "deleting: " << std::endl;
			std::for_each
			(
				d->blocks_.at(source_block_list_num).begin(),
				d->blocks_.at(source_block_list_num).end(),
				std::cerr << boost::lambda::bind(&Block::get_name_sys, boost::lambda::_1) << " "
			);
			std::cerr << std::endl;
#endif
			d->blocks_.erase(d->blocks_.begin() + source_block_list_num);
		}
	} else
	{
		if (*source_block_pos > *sink_block_pos)
		/* long story... i will coment on that later. */
		{
 			d->blocks_[source_block_list_num].insert
			(
				source_block_pos+1,
				*sink_block_pos
			);

			d->blocks_[source_block_list_num].erase
			(
				std::find_if
				(
					d->blocks_[source_block_list_num].begin(),
					d->blocks_[source_block_list_num].end(),
					block_sink == boost::lambda::bind(&Block::get_name_sys, boost::lambda::_1)
				)
			);
		}
	}

#ifndef NDEBUG
	std::cerr << "size of block-list-list: " << d->blocks_.size() << std::endl;
#endif

	/* make the send() method of the source port call the right method of the sink port */
	source_port_it->connect(*sink_port_it, d->signal_buffer_count_);
	
	uint32_t curr_sig_buffer =
		d->create_signal_buffer(source_port_it->get_type(), source_port_it->get_frame_size());

	d->set_buffer_ptrs(*source_port_it, *sink_port_it, &d->signal_buffers_[curr_sig_buffer]);

}



void System::wakeup_sys(uint32_t times)
{
	H_D(System)

	if(d->blocks_.size() == 1)
	{
		for(uint32_t i=0; i<times; i++)
		{
			std::for_each
			(
				d->blocks_.front().begin(),
				d->blocks_.front().end(),
				bind(&Block::wakeup, _1)
			);
		}
	}
}



/* sometimes you have to get by without anonymous functions ... */
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
	get_buffer_factory_.Register(integer, &get_buffer< IntegerSignal >);
	signal_factory_.Register(integer, bind< IntegerSignal* >(new_ptr< IntegerSignal >(), _1));

	/* RealSignal and real_t */
	get_buffer_factory_.Register(real, &get_buffer< RealSignal >);
	signal_factory_.Register(real, bind< RealSignal* >(new_ptr< RealSignal >(), _1));

	/* ComplexSignal and complex_t */
	get_buffer_factory_.Register(complex, &get_buffer< ComplexSignal >);
	signal_factory_.Register(complex, bind< ComplexSignal* >(new_ptr< ComplexSignal >(), _1));

	/* StringSignal and string_t */
	get_buffer_factory_.Register(string, &get_buffer< StringSignal >);
	signal_factory_.Register(string, bind< StringSignal* >(new_ptr< StringSignal >(), _1));

	/* BitSignal and logical_t */
	get_buffer_factory_.Register(logical, &get_buffer< BitSignal >);
	signal_factory_.Register(logical, bind< BitSignal* >(new_ptr< BitSignal >(), _1));
}



uint32_t SystemImpl::create_signal_buffer(type_t type, uint32_t size)
{
	signal_buffers_.push_back(signal_factory_.CreationFunction(type)(size));
	return signal_buffer_count_++;
}



void SystemImpl::set_buffer_ptrs(OutPort& out, InPort& in, Signal* s)
{
	void* (*f) (Signal*);
	f = get_buffer_factory_.CreationFunction(out.get_type());

	out.get_buffer_ptr = bind(f, s);
	in.get_buffer_ptr = bind(f, s);
}
