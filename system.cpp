#include "system.hpp"
#include "exceptions.hpp"
#include "signal.hpp"
#include "block.hpp"
#include "variable.hpp"
#include "port.hpp"
#include "symtab.hpp"
#include "types.hpp"
#include "exec_matrix.hpp"

#include <boost/preprocessor/repetition.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <map>
#include <set>
#include <iostream>

#include "system.ipp"


using boost::bind;
using boost::lambda::new_ptr;
using boost::lambda::delete_ptr;



SystemImpl::SystemImpl() :
	signal_buffers_(),
	signal_buffer_count_(0),
	signal_factory_(),
	get_buffer_factory_(),
	simulation_time_(0.0),
	symtab_(),
	exec_m_()
{
	register_basic_types();
}



SystemImpl::~SystemImpl()
{
#ifndef NDEBUG
	std::cout << "Bye from System." << std::endl;
#endif
	for_each
	(
		signal_buffers_.begin(),
		signal_buffers_.end(),
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



namespace
{
	template< class SignalT >
	inline void* get_buffer(Signal *s)
	{
		return static_cast< SignalT* >(s)->get_data();
	}
}



void SystemImpl::register_basic_types()
{
// 	this macro inserts entries for all Singal types
//	for integer valued signals, the expansion would look like this:
// 	get_buffer_factory_.insert(std::make_pair(integer, &get_buffer< IntegerSignal >));
// 	signal_factory_.insert(std::make_pair(integer, bind< IntegerSignal* >(new_ptr< IntegerSignal >(), _1)));

#define BOOST_PP_DEF(z, I, _) 									\
	get_buffer_factory_.insert(std::make_pair(BOOST_PP_ARRAY_ELEM(1, SIGNAL_TYPE(I)),	\
		&get_buffer< BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I)) >)); 			\
	signal_factory_.insert(std::make_pair(BOOST_PP_ARRAY_ELEM(1, SIGNAL_TYPE(I)),		\
		bind< BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I))* >(new_ptr< BOOST_PP_ARRAY_ELEM(2, SIGNAL_TYPE(I)) >(), _1)));

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _);

#undef BOOST_PP_DEF
}



uint32_t SystemImpl::create_signal_buffer(type_t type, uint32_t size)
{
#ifndef NDEBUG
	std::cout << "  creating signal buffer no. " << signal_buffer_count_ << ":" << std::endl;
	std::cout << "    type: " << type << ", size: " << size << std::endl;
#endif
	signal_buffers_.push_back(signal_factory_[type](size));
	return signal_buffer_count_++;
}



void SystemImpl::set_buffer_ptrs(OutPort& out, InPort& in, Signal* s)
{
	void* (*f) (Signal*);

#ifndef NDEBUG
	std::cout << "    setting buffer aquiration functions for ports '" << out.get_name() << "' and '" << in.get_name() << "'" << std::endl;
#endif

	f = get_buffer_factory_[out.get_type()];

	out.get_buffer_ptr = bind(f, s);
	in.get_buffer_ptr = bind(f, s);
}



void System::add_block(Block * const b, const std::string& name_sys)
{
	H_D(System)
	d->add_block_impl(b, name_sys);
}



void SystemImpl::add_block_impl(Block * const b, const std::string& name_sys)
{

	if(exec_m_.block_exists(name_sys))
	{
		delete b;
		throw DuplicateBlockNameException(name_sys);
	}

	// if we make it here, we can set up and add the blocks to the system
	if (!b->is_configured())
	{
		delete b;
		throw BlockNotConfiguredException(name_sys);
	}

	// give it its unique name
	b->set_name_sys(name_sys);

	b->setup_input_ports();	
	b->setup_output_ports();

	exec_m_.store_block(b, name_sys);
}



template< typename SystemT >
struct PlaceBlockAction
{
	PlaceBlockAction(const std::string& block_curr, SystemT& sys) : block_curr_(block_curr), sys_(sys) { }

	void operator()(const std::string& block_next) const
	{
		if(not sys_->exec_m_.block_is_placed(block_next))
		{
#ifndef NDEBUG
			std::cout << std::endl << "  placing block '" << block_next << "' after '" << block_curr_ << "'" << std::endl;
#endif
			sys_->exec_m_.place_block(block_next, block_curr_);
#ifndef NDEBUG
			std::cout << sys_->exec_m_;
#endif
			sys_->linearize(block_next);
		}
#ifndef NDEBUG
		else
		{
			std::cout << "block '" << block_next << "' has already been placed. skipping..." << std::endl;
		}
#endif
	}

	const std::string& block_curr_;
	SystemT& sys_;

	typedef void result_type;
};



template< typename SystemT >
inline PlaceBlockAction< SystemT > place_block_a(const std::string& block_start, SystemT& sys)
{
	return PlaceBlockAction< SystemT >(block_start, sys);
}



template< class SystemT >
struct MakeConnectionAction
{
	MakeConnectionAction(SystemT* sys) : sys_(sys) { }

	template< typename PairT >
	void operator()(const PairT& ports)
	{
		OutPort::store_t::iterator source_port_it = ports.first;
		InPort::store_t::iterator sink_port_it = ports.second;

		source_port_it->connect(*sink_port_it, sys_->signal_buffer_count_);

		sys_->create_signal_buffer(source_port_it->get_type(), source_port_it->get_frame_size());

		sys_->set_buffer_ptrs(*source_port_it, *sink_port_it, sys_->signal_buffers_[sys_->signal_buffer_count_-1]);
	}

	SystemT* sys_;

	typedef void result_type;
};



template< class SystemT >
inline MakeConnectionAction< SystemT > make_connections_a(SystemT* sys)
{
	return MakeConnectionAction< SystemT >(sys);
}



template< class SystemT >
struct SignalAttributePropagationAction
{
	SignalAttributePropagationAction(SystemT sys) : sys_(sys) { };

	template< typename StageT >
	void operator()(const StageT& stage_curr) const
	{
		Block * const b = stage_curr.get_paths().front().front();
		b->setup_output_ports();

		std::for_each
		(
			b->connect_calls.begin(),
			b->connect_calls.end(),
			make_connections_a(sys_)
		);
// 		b->connect_calls.clear();
	}

	SystemT sys_;
};



template< class SystemT >
inline SignalAttributePropagationAction< SystemT > create_buffers_and_stuff_a(SystemT sys)
{
	return SignalAttributePropagationAction< SystemT >(sys);
}



void SystemImpl::linearize(const std::string& block_start)
{
	std::for_each
	(
		exec_m_[block_start]->get_connections().begin(),
		exec_m_[block_start]->get_connections().end(),
		place_block_a(block_start, this)
	);
}



void SystemImpl::propagate_signal_attributes()
{
	std::for_each
	(
		exec_m_.get_stages().begin(),
		exec_m_.get_stages().end(),
		create_buffers_and_stuff_a(this)
	);
}



void System::connect_ports(const std::string & block_source,
			   const std::string & port_source,
			   const std::string & block_sink,
			   const std::string & port_sink)
{
	H_D(System)
#ifndef NDEBUG
	std::cerr << "connect " << block_source << "->" << block_sink << std::endl;
#endif
	OutPort::store_t::iterator source_port_it;
	InPort::store_t::iterator sink_port_it;

	// ensure that the given block and port name are valid.
	// if signal types and frame sizes are compatible is checked at a later time.

	// search for both source and sink block:
	// 1) source block
	if(not d->exec_m_.block_exists(block_source))
	{
		throw InvalidBlockNameException(block_source);
	}

	// so we found our source block. now let's see if the given port name was valid
	source_port_it =
		std::find_if
		(
			d->exec_m_[block_source]->get_port_list< OutPort >()->begin(),
			d->exec_m_[block_source]->get_port_list< OutPort >()->end(),
			bind(&OutPort::get_name, _1) == port_source
		);

	// unfortunately the given port name was invalid
	if (source_port_it == d->exec_m_[block_source]->get_port_list< OutPort >()->end())
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
	if(not d->exec_m_.block_exists(block_sink))
	{
		throw InvalidBlockNameException(block_sink);
	}

	// so we found our sink block. now let's see if the given port name was valid
	sink_port_it =
		std::find_if
		(
			d->exec_m_[block_sink]->get_port_list< InPort >()->begin(),
			d->exec_m_[block_sink]->get_port_list< InPort >()->end(),
			bind(&InPort::get_name, _1) == port_sink
		);

	// unfortunately the given port name was invalid
	if (sink_port_it == d->exec_m_[block_sink]->get_port_list< InPort >()->end())
	{
		throw InvalidPortNameException(port_sink);
	}

	/*
	 *  all provided block- and port names were valid, now let's connect them
	 */

	d->exec_m_[block_source]->connect_calls.push_back(std::make_pair(source_port_it, sink_port_it));
	d->exec_m_[block_source]->add_connection(block_sink);
}



void System::initialize()
{
	H_D(System);

	// in case of subsystems this does not help very much
	std::vector< Block * > start_blocks = d->exec_m_.find_start_blocks();

	for
	(
		std::vector< Block * >::iterator source_block = start_blocks.begin();
		source_block != start_blocks.end();
		source_block++
	)
	{
		std::string start_block_name = (*source_block)->get_name_sys();

		if(d->exec_m_.block_is_placed(start_block_name))
		{
#ifndef NDEBUG
			std::cout << "block named '" << start_block_name << "' has already been placed." << std::endl;
#endif
			continue;
		}
#ifndef NDEBUG
		std::cout << "starting linearization with block '" << start_block_name << "'." << std::endl;
#endif

		d->exec_m_.add_block(start_block_name);

#ifndef NDEBUG
		std::cout << std::endl << "linearizing system..." << std::endl;
#endif

		d->linearize(start_block_name);
	}

	if(start_blocks.empty())
	{
#ifndef NDEBUG
		std::cout << "no start blocks!" << std::endl;
#endif
	}


#ifndef NDEBUG
	std::cout << "propagating signal attributes and creating signal buffers" << std::endl;
#endif

	d->propagate_signal_attributes();

#ifndef NDEBUG
	std::cout << std::endl << "combining execution stages..." << std::endl;
#endif

	d->exec_m_.combine_stages();

#ifndef NDEBUG
	std::cout << "parallelizing..." << std::endl;
#endif

	d->exec_m_.parallelize();

	d->exec_m_.init();
}



void System::wakeup_sys(uint32_t times)
{
	H_D(System);

	for(uint32_t t=0; t<times; ++t)
	{
		d->exec_m_.exec();
	}
}



void System::add_variable( const std::string& name, const Variable& var )
{
	H_D(System)
#ifndef NDEBUG
	bool var_name_is_available =
#endif
	d->symtab_.add_var(name, var);
	assert(var_name_is_available == true);
#ifndef NDEBUG
	std::cout << "added variable '" << name << "'." << std::endl;
#endif
}



Variable System::get_variable(const std::string& name) const
{
	return d_func()->symtab_.get_var(name);
}
