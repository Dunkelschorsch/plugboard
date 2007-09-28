#include "system.hpp"
#include "exceptions.hpp"
#include "signal.hpp"
#include "block.hpp"
#include "variable.hpp"
#include "port.hpp"
#include "symtab.hpp"
#include "types.hpp"


#include <boost/preprocessor/repetition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <map>
#include <set>
#include <iostream>


using boost::bind;
using boost::function;
using boost::lambda::new_ptr;
using boost::lambda::delete_ptr;


class ExecutionStage
{
public:
	ExecutionStage();
	~ExecutionStage();

	void add_path(Block::store_t *blocks);

protected:
	std::deque< Block::store_t > blocks_;
	std::vector< uint32_t > dependencies_;
};



ExecutionStage::ExecutionStage() :
	blocks_(),
	dependencies_()
{

}



ExecutionStage::~ExecutionStage()
{

}





struct SystemImpl
{
	SystemImpl();

	~SystemImpl();

	inline uint32_t create_signal_buffer(type_t type, uint32_t size);

	void register_basic_types();

	inline void set_buffer_ptrs(OutPort& out, InPort& in, Signal *s);

	void add_block_impl(Block *b, const std::string& name_sys);

	Signal::store_t signal_buffers_;

	uint32_t signal_buffer_count_;

	typedef std::map< type_t, function< Signal*(size_t) > > signal_factory_t;

	signal_factory_t signal_factory_;

	typedef std::map< type_t, void* (*)(Signal*) > get_buffer_factory_t;

	get_buffer_factory_t get_buffer_factory_;

	double simulation_time_;

	Symtab symtab_;

	typedef std::deque< std::deque< Block::store_t > > block_list_t;

	Block::store_t initial_block_list_;

	block_list_t blocks_;

	std::vector< std::string > block_names_;

	std::set< std::string > placed_blocks_;

	void linearize(const std::string& block_start);

	void combine_stages();

	void parallelize();


#ifndef NDEBUG
	void show_sys();
#endif

	void execute_row(uint32_t stage_num, uint32_t row_num);
};



SystemImpl::SystemImpl() :
	signal_buffers_(),
	signal_buffer_count_(0),
	signal_factory_(),
	get_buffer_factory_(),
	simulation_time_(0.0),
	symtab_(),
	blocks_(),
	block_names_()
{
	register_basic_types();
}



SystemImpl::~SystemImpl()
{
	for(uint32_t i=0; i<blocks_.size(); ++i)
	{
		for(uint32_t j=0; j<blocks_[i].size(); ++j)
		{
			for(uint32_t k=0; k<blocks_[i][j].size(); ++k)
			{
				delete blocks_[i][j][k];
			}
		}
	}
	for(uint32_t i=0; i<signal_buffer_count_; ++i)
	{
		delete signal_buffers_[i];
	}
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
	// determine if the given block name already exists
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
		throw DuplicateBlockNameException(name_sys);
	}

	// if we make it here, we can set up and add the blocks to the system
	if (!b->is_configured())
	{
		// maybe the block does not need to be configured
		if (!(b->get_params().size() == 0))
		{
			delete b;
			throw BlockNotConfiguredException(name_sys);
		}
	}

	// give it its unique name
	b->set_name_sys(name_sys);

	if(!b->setup_input_ports())
	{

	}

	if(!b->setup_output_ports())
	{

	}

	initial_block_list_.push_back(b);

#ifndef NDEBUG
	std::cerr << initial_block_list_.size()-1 << ": "
		<< initial_block_list_.back()->get_name_sys() <<  " type: "
		<< initial_block_list_.back()->get_name() << std::endl;

#endif

	// add the block's name to prevent further usage
	block_names_.push_back(name_sys);
}



bool is_left_terminated(Block::store_t v)
{
	return v.front()->get_num_input_ports() > 1 || v.front()->get_num_input_ports() == 0;
}



bool is_right_terminated(Block::store_t v)
{
	return v.back()->get_num_output_ports() > 1 || v.back()->get_num_output_ports() == 0;
}



bool is_left_terminated(Block *b)
{
	return b->get_num_input_ports() > 1 || b->get_num_input_ports() == 0;
}



bool is_right_terminated(Block *b)
{
	return b->get_num_output_ports() > 1 || b->get_num_output_ports() == 0;
}



void SystemImpl::linearize(const std::string& block_start)
{
	Block::store_t::iterator left_block;

	std::deque< std::deque< Block::store_t > >::iterator stage_it;
	std::deque< Block::store_t >::iterator row_it;
	std::set< std::string >::iterator block_it;

	for(stage_it = blocks_.begin(); stage_it != blocks_.end(); stage_it++)
	{
		for(row_it = stage_it->begin(); row_it != stage_it->end(); row_it++)
		{
			left_block = std::find_if
			(
				row_it->begin(),
				row_it->end(),
				bind(&Block::get_name_sys, _1) == block_start
			);
			if (left_block != row_it->end())
			{
				for
				(
					block_it = (*left_block)->get_connections().begin();
					block_it != (*left_block)->get_connections().end();
					block_it++
				)
				{
					if(placed_blocks_.find(*block_it) != placed_blocks_.end())
					{
#ifndef NDEBUG
						std::cout << *block_it << " has already been placed. skipping" << std::endl;
#endif
					}
					else
					{
						placed_blocks_.insert(*block_it);
#ifndef NDEBUG
						std::cout << "placing " << *block_it << " after " << block_start << std::endl;
#endif
						Block::store_t::iterator right_block =
							std::find_if
							(
								initial_block_list_.begin(),
								initial_block_list_.end(),
								bind(&Block::get_name_sys, _1) == *block_it
							);

						std::deque< Block::store_t > q;
						Block::store_t bs;
						bs.push_back(*right_block);
						q.push_back(bs);

						blocks_.insert(stage_it+1, q);

						linearize(*block_it);
					}
				}
			}
		}
	}
}




void SystemImpl::combine_stages()
{

	std::deque< std::deque< Block::store_t > >::iterator stage_it;
	std::set< std::string >::iterator conn_it;
	
	for(stage_it = blocks_.begin(); stage_it != blocks_.end(); stage_it++)
	{
#ifndef NDEBUG
		std::cout << "looking at: " << (stage_it)->front().back()->get_name_sys() << std::endl;
#endif
		conn_it = stage_it->front().back()->get_connections().find((stage_it+1)->front().front()->get_name_sys());
		
		while
		(
			conn_it != stage_it->front().back()->get_connections().end() &&
			!is_right_terminated(stage_it->front()) &&
			!is_left_terminated((stage_it+1)->front())
		)
		{
#ifndef NDEBUG
			std::cout << "interesting: " << (stage_it)->front().back()->get_name_sys() << std::endl;
#endif
			stage_it->front().push_back((stage_it+1)->front().front());
			blocks_.erase(stage_it+1);

			conn_it = stage_it->front().back()->get_connections().find((stage_it+1)->front().front()->get_name_sys());
		}
	}
}



void SystemImpl::parallelize()
{
	std::deque< std::deque< Block::store_t > >::iterator stage_it;
	std::set< std::string >::iterator conn_it;

	for(stage_it = blocks_.begin(); stage_it != blocks_.end()-1; stage_it++)
	{
#ifndef NDEBUG
		std::cout << "parallel looking at: " << (stage_it)->front().back()->get_name_sys() << std::endl;
#endif
		conn_it = stage_it->front().back()->get_connections().find((stage_it+1)->front().front()->get_name_sys());

#ifndef NDEBUG
		std::cout << "connected to " << (stage_it+1)->front().front()->get_name_sys() << "?" << std::endl;
#endif
		if(conn_it == stage_it->front().back()->get_connections().end())
		{

			Block::store_t qq((stage_it+1)->front());
#ifndef NDEBUG
			std::cout << "parallel interesting..." << qq.front()->get_name_sys() << std::endl;
#endif
			stage_it->push_back(qq);
			blocks_.erase(stage_it+1);

			// that one is not nice but necessary
			if(stage_it ==  blocks_.end()-1)
				break;
		}
	}
}



void System::connect_ports(const std::string & block_source,
			   const std::string & port_source,
			   const std::string & block_sink,
                           const std::string & port_sink)
{

	H_D(System);
#ifndef NDEBUG
	std::cerr << "connect " << block_source << " --> " << block_sink << std::endl;
#endif
	OutPort::store_t::iterator source_port_it;
	InPort::store_t::iterator sink_port_it;

	Block::store_t::iterator source_block_it;
	Block::store_t::iterator sink_block_it;

	// search for both source and sink block:
	// 1) source block
	source_block_it =
		std::find_if
		(
			d->initial_block_list_.begin(),
			d->initial_block_list_.end(),
			bind(&Block::get_name_sys, _1) == block_source
		);

	if(source_block_it == d->initial_block_list_.end())
	{
		throw InvalidBlockNameException(block_source);
	}

	// so we found our source block. now let's see if the given port name was valid
	source_port_it =
		std::find_if
		(
			(*source_block_it)->get_outport_list().begin(),
			(*source_block_it)->get_outport_list().end(),
			bind(&OutPort::get_name, _1) == port_source
		);

	// unfortunately the given port name was invalid
	if (source_port_it == (*source_block_it)->get_outport_list().end())
	{
		throw InvalidPortNameException(port_source);
	}

	if (source_port_it->send != 0)
	{
		// the ports exists, but has already been connected
		std::cerr << "already connected!" << std::endl;
		return;
	}

	// 2) sink block
	sink_block_it =
		std::find_if
		(
			d->initial_block_list_.begin(),
			d->initial_block_list_.end(),
			bind(&Block::get_name_sys, _1) == block_sink
		);

	if(sink_block_it == d->initial_block_list_.end())
	{
		throw InvalidBlockNameException(block_sink);
	}

	// so we found our sink block. now let's see if the given port name was valid
	sink_port_it =
		std::find_if
		(
			(*sink_block_it)->get_inport_list().begin(),
			(*sink_block_it)->get_inport_list().end(),
			bind(&InPort::get_name, _1) == port_sink
		);

	// unfortunately the given port name was invalid
	if (sink_port_it == (*sink_block_it)->get_inport_list().end())
	{
		throw InvalidPortNameException(port_sink);
	}


	/*
	 *  all provided block- and port names were valid, now let's connect them
	 */
	
	source_port_it->connect(*sink_port_it, d->signal_buffer_count_);
	
	uint32_t curr_sig_buffer =
		d->create_signal_buffer(source_port_it->get_type(), source_port_it->get_frame_size());

	d->set_buffer_ptrs(*source_port_it, *sink_port_it, d->signal_buffers_[curr_sig_buffer]);

	(*source_block_it)->add_connection(block_sink);
}



void System::initialize()
{
	H_D(System);

	Block::store_t::iterator start_block_it =
		std::find_if
		(
			d->initial_block_list_.begin(),
			d->initial_block_list_.end(),
			bind(&Block::get_num_input_ports, _1) == 0
		);

#ifndef NDEBUG
	std::cout << "starting with block named '" << (*start_block_it)->get_name_sys() << "'." << std::endl;
#endif
	std::deque< Block::store_t > q;
	Block::store_t bs;
	bs.push_front(*start_block_it);
	q.push_back(bs);
	d->blocks_.push_front(q);

	d->linearize((*start_block_it)->get_name_sys());

	d->combine_stages();

#ifndef NDEBUG
	d->show_sys();
#endif

	d->parallelize();

#ifndef NDEBUG
	d->show_sys();
#endif
	for(uint32_t i=0; i<d->blocks_.size(); ++i)
		for(uint32_t j=0; j<d->blocks_[i].size(); ++j)
			for(uint32_t k=0; k<d->blocks_[i][j].size(); ++k)
				d->blocks_[i][j][k]->initialize();
}



#ifndef NDEBUG
void SystemImpl::show_sys()
{
	for(uint32_t i=0; i<blocks_.size(); ++i)
	{
		std::cout << "Stage: " << i << std::endl;
		for(uint32_t j=0; j<blocks_[i].size(); ++j)
		{
			std::cout << "  Row: " << j << std::endl;
			for(uint32_t k=0; k<blocks_[i][j].size(); ++k)
			{
				std::cout << "   Blockname: " << blocks_[i][j][k]->get_name_sys() << std::endl;
			}
		}
	}
}
#endif



void SystemImpl::execute_row(uint32_t stage_num, uint32_t row_num)
{
	std::for_each
	(
		blocks_[stage_num][row_num].begin(),
		blocks_[stage_num][row_num].end(),
		bind(&Block::wakeup, _1)
	);
}



void System::wakeup_sys(uint32_t times)
{
	H_D(System);

#ifndef NO_THREADS
	boost::thread_group threads;
#endif
	uint32_t num_blocks;
	for(uint32_t t=0; t<times; ++t)
	// iterate through stages
	for(uint32_t i=0; i<d->blocks_.size(); ++i)
	{
		num_blocks = d->blocks_[i].size();
		// do not create a thread when not necessary
		if(num_blocks > 1)
		{
			// iterate through rows
			for(uint32_t j=0; j<num_blocks; ++j)
			{
				 // iterate through blocks of a row
#ifndef NO_THREADS
				threads.create_thread(bind(&SystemImpl::execute_row, d, i, j));
#else
				d->execute_row(i, j);
#endif
			}
#ifndef NO_THREADS
			threads.join_all();
#endif
		}
		else
		{
			d->execute_row(i, 0);
		}
	}
}



namespace
{
	template< class T >
	void* get_buffer(Signal *s)
	{
		return static_cast < T* >(s)->get_data();
	}
}



void SystemImpl::register_basic_types()
{
// 	this macro inserts entries for all Singal types
//	for integer values signal the expansion would look like this:
// 	get_buffer_factory_.insert(std::make_pair(integer, &get_buffer< IntegerSignal >));
// 	signal_factory_.insert(std::make_pair(integer, bind< IntegerSignal* >(new_ptr< IntegerSignal >(), _1)));

#define BOOST_PP_DEF(z, I, _) 									\
	get_buffer_factory_.insert(std::make_pair(BOOST_PP_ARRAY_ELEM(1, SIGNAL_TYPE(I)),	\
		&get_buffer< BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I)) >)); 			\
	signal_factory_.insert(std::make_pair(BOOST_PP_ARRAY_ELEM(1, SIGNAL_TYPE(I)),		\
		bind< BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I))* >(new_ptr< BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I)) >(), _1)));

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF
}



uint32_t SystemImpl::create_signal_buffer(type_t type, uint32_t size)
{
	signal_buffers_.push_back(signal_factory_[type](size));
	return signal_buffer_count_++;
}



void SystemImpl::set_buffer_ptrs(OutPort& out, InPort& in, Signal* s)
{
	void* (*f) (Signal*);
	f = get_buffer_factory_[out.get_type()];

	out.get_buffer_ptr = bind(f, s);
	in.get_buffer_ptr = bind(f, s);
}
