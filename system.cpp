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

	void add_block_test(Block *b, const std::string& name_sys);

	Block::store_t blocks_;

	Signal::store_t signal_buffers_;

	uint32_t signal_buffer_count_;

	typedef Factory< void*, type_t, function< Signal*(size_t) > > signal_factory_t;

	signal_factory_t signal_factory_;

	typedef Factory< void*, type_t, function< void*(Signal*) > > get_buffer_factory_t;

	get_buffer_factory_t get_buffer_factory_;

	double simulation_time;

	Symtab symtab_;

	std::vector< std::vector < Block* > > blocks_test_;

	std::vector< std::string > block_names_;
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



void System::add_block(Block *b, const std::string& name_sys)
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

	//d->blocks_.push_back(b);
	
	d->add_block_test(b, name_sys);

}



void SystemImpl::add_block_test(Block *b, const std::string& name_sys)
{
	using boost::lambda::_1;
	std::vector< std::string >::const_iterator it =
		std::find_if(block_names_.begin(), block_names_.end(), _1 == name_sys);

	if(it != block_names_.end())
	{
		delete b;
		throw duplicate_block_name_error(name_sys+"hump");
	}


	std::vector< Block* > b_temp;
	b_temp.push_back(b);

	blocks_test_.push_back(b_temp);
#ifndef NDEBUG
	std::cout << blocks_test_.size()-1 << ": "
		<< blocks_test_.back().front()->get_name_sys() <<  " type: "
		<< blocks_test_.back().front()->get_name() << std::endl;
#endif
	block_names_.push_back(name_sys);
}



void System::connect_ports(const std::string & block_source,
			   const std::string & port_source,
			   const std::string & block_sink,
                           const std::string & port_sink)
{
	H_D(System)



	/* try to retrieve a block */
	std::cout << "connect " << block_source << " --> " << block_sink << std::endl;

	uint32_t source_block_list_num, sink_block_list_num;

	for(uint32_t i=0; i<d->blocks_test_.size(); i++)
	{
		std::vector < Block* >::iterator source_it = std::find_if(d->blocks_test_[i].begin(), d->blocks_test_[i].end(),
			bind(&Block::get_name_sys, _1) == block_source);

		std::vector < Block* >::iterator sink_it = std::find_if(d->blocks_test_[i].begin(), d->blocks_test_[i].end(),
			bind(&Block::get_name_sys, _1) == block_sink);

		
		if(source_it != d->blocks_test_[i].end())
		{
			source_block_list_num = i;
			/* move a block from one vector to the other */
			#if 0
			
			d->blocks_test_[0].push_back(*it);
			
			d->blocks_test_.pop_back();
			std::cout << d->blocks_test_.size() << std::endl;

			d->blocks_test_[0][0]->wakeup();
			d->blocks_test_[0][1]->wakeup();
			#endif
		}
		if(sink_it != d->blocks_test_[i].end())
		{
			sink_block_list_num = i;

		}
	}
	std::cout << "found source block in vector no. " << source_block_list_num << std::endl;
	std::cout << "found sink block in vector no. " << sink_block_list_num << std::endl;

	d->blocks_test_[source_block_list_num].insert(d->blocks_test_[source_block_list_num].end(), d->blocks_test_[sink_block_list_num].begin(), d->blocks_test_[sink_block_list_num].end());


	std::cout << "deleting: " << std::endl;
	
	for(std::vector< Block* >::iterator it = d->blocks_test_.at(sink_block_list_num).begin(); it != d->blocks_test_.at(sink_block_list_num).end(); it++)
	{
		std::cout << (*it)->get_name_sys() << " ";
		
	}
	std::cout << std::endl;
	//d->blocks_test_.at(sink_block_list_num).clear();
	d->blocks_test_.erase(d->blocks_test_.begin()+sink_block_list_num);

	//std::for_each(d->blocks_test_[source_block_list_num].begin(), d->blocks_test_[source_block_list_num].end(),
	//	bind(&Block::wakeup, _1));

	std::cout << "size: " << d->blocks_test_.size() << std::endl;

	if(d->blocks_test_.size() == 1)
		std::for_each(d->blocks_test_.front().begin(), d->blocks_test_.front().end(),
			bind(&Block::wakeup, _1));

	return ;

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
