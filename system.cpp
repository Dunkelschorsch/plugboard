/*!
 * \file
 * \brief
 * \author Armin Schmidt
 *
 * ----------------------------------------------------------------------------
 *
 * PlugBoard - A versatile communication simulation framework
 * Copyright (C) 2007-2008  Armin Schmidt
 *
 * This file is part of PlugBoard.
 *
 * PlugBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PlugBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PlugBoard.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ----------------------------------------------------------------------------
 */

#include <map>
#include <set>
#include <iostream>

#include <boost/preprocessor/repetition.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "system.hpp"
#include "exec_matrix.hpp"
#include "types/base.hpp"
#include "variable/variable.hpp"
#include "signal/signal.hpp"
#include "symtab.hpp"
#include "exception/block.hpp"
#include "exception/port.hpp"
#include "port/port.hpp"
#include "block/block.hpp"
#include "block/source.hpp"
#include "block/sink.hpp"


#define PB_DEBUG_MESSAGE_COLOUR \033[22;32m
#define PB_DEBUG_MESSAGE_SOURCE System

#include "colour_debug.hpp"

using namespace plugboard;

using boost::bind;
using boost::lambda::new_ptr;
using boost::function;

typedef pimpl< System >::implementation SystemImpl;


template<>
struct pimpl< System >::implementation
{
	implementation();

	void register_basic_types();

	uint32_t create_signal_buffer(type_t type, uint32_t size);
	void set_buffer_ptrs(OutPort& out, InPort& in, Signal* s);

	void linearize(const std::string& block_start);

	void propagate_signal_attributes();

	void execute_row(uint32_t stage_num, uint32_t row_num);

	Signal::store_t signal_buffers;
	uint32_t signal_buffer_count;

	typedef std::map< type_t, function< Signal*(size_t) > > signal_assoc_t;
	typedef std::map< type_t, void* (*)(Signal*) > get_buffer_assoc_t;

	signal_assoc_t signal_factory;
	get_buffer_assoc_t get_buffer_factory;

	double simulation_time;

	Symtab symtab;

	ExecutionMatrix exec_m;
};


namespace actions
{
  struct PlaceBlock
  {
    PlaceBlock(const std::string& block_curr, SystemImpl& sys) : block_curr_(block_curr), sys_(sys) { }

    void operator()(const std::string& block_next) const
    {
      if (not sys_.exec_m.block_is_placed(block_next))
      {
	PB_DEBUG_MESSAGE("placing block '" << block_next << "' after '" << block_curr_ << "'")

	sys_.exec_m.place_block(block_next, block_curr_);

	PB_DEBUG_MESSAGE("setup looks like this:" << std::endl << sys_.exec_m)

	sys_.linearize(block_next);
      }
      else
      {
	PB_DEBUG_MESSAGE("block '" << block_next << "' has already been placed. skipping...")
      }
    }

    const std::string& block_curr_;
    SystemImpl& sys_;

    typedef void result_type;
  };


  struct MakeConnection
  {
    MakeConnection(SystemImpl& sys) : sys_(sys) { }

    template< typename PairT >
    void operator()(const PairT& ports)
    {
      OutPort::store_t::iterator source_port_it = ports.first;
      InPort::store_t::iterator sink_port_it = ports.second;

      source_port_it->connect(*sink_port_it, sys_.signal_buffer_count);

      sys_.create_signal_buffer(source_port_it->get_type(), source_port_it->get_frame_size());
      sys_.set_buffer_ptrs(*source_port_it, *sink_port_it, sys_.signal_buffers[sys_.signal_buffer_count-1].get());
    }

    SystemImpl& sys_;

    typedef void result_type;
  };


  struct SignalAttributePropagation
  {
    SignalAttributePropagation(SystemImpl& sys) : sys_(sys) { };

    template< typename StageT >
    void operator()(const StageT& stage_curr) const
    {
      source_ptr const src = boost::dynamic_pointer_cast< Source >(stage_curr.get_paths().front().front());

      if (src)
      {
	src->call_setup_output_ports();
	std::for_each
	(
	  src->connect_calls.begin(),
	  src->connect_calls.end(),
	  actions::MakeConnection(sys_)
	);
      }
    }

    SystemImpl& sys_;
    
    typedef void result_type;
  };


  struct SourceBlocksToFront
  {
    bool operator()(const block_ptr b1, const block_ptr b2) const
    {
      uint16_t num_inputs_b1 = 0, num_outputs_b1 = 0;
      uint16_t num_inputs_b2 = 0, num_outputs_b2 = 0;
      const_source_ptr src;
      const_sink_ptr sink;

      src = boost::dynamic_pointer_cast< const Source >(b1);
      num_outputs_b1 = src->get_num_output_ports();

      src = boost::dynamic_pointer_cast<  const Source >(b2);
      num_outputs_b2 = src->get_num_output_ports();

      // source-only blocks are always preferred
      sink = boost::dynamic_pointer_cast< const Sink >(b1);
      if (sink)
	num_inputs_b1 = sink->get_num_input_ports();
      else
	return true;

      sink = boost::dynamic_pointer_cast< const Sink >(b2);
      if (sink)
	num_inputs_b2 = sink->get_num_input_ports();
      else
	return false;

      return num_outputs_b1-num_inputs_b1 > num_outputs_b2-num_inputs_b2;
    }
    
    typedef bool result_type;
  };
}


template< class SignalT >
inline void* get_sig(Signal *s)
{
  return static_cast< SignalT* >(s)->get_data();
}


System::System() : base() { }


System& System::instance( )
{
	System & s = singleton::instance();
	return s;
}


void System::add_block(block_ptr const b, const std::string& name_sys)
{
	implementation& impl = **this;

	if(impl.exec_m.block_exists(name_sys))
	{
		throw DuplicateBlockNameException(name_sys);
	}

	// if we make it here, we can set up and add the blocks to the system
	assert(b->is_configured());

	// give it its unique name
	b->set_name_sys(name_sys);

	if(boost::dynamic_pointer_cast< Sink >(b))
	{
		boost::dynamic_pointer_cast< Sink >(b)->call_setup_input_ports();
	}

	if(boost::dynamic_pointer_cast< Source >(b))
	{
		boost::dynamic_pointer_cast< Source >(b)->call_setup_output_ports();
	}

	impl.exec_m.store_block(b, name_sys);
}


void System::add_variable( const std::string& name, const Variable& var )
{
#ifndef NDEBUG
	bool var_name_is_available =
#endif
	(*this)->symtab.add_var(name, var);
	assert(var_name_is_available == true);
	PB_DEBUG_MESSAGE("added variable '" << name << "'.")
}


const Variable& System::get_variable(const std::string& name) const
{
	return (*this)->symtab.get_var(name);
}


void System::connect_ports(const std::string & block_source,
			const std::string & port_source,
			const std::string & block_sink,
			const std::string & port_sink)
{
	implementation& impl = **this;

	OutPort::store_t::iterator source_port_it;
	InPort::store_t::iterator sink_port_it;

	// ensure that the given block and port name are valid.
	// if signal types and frame sizes are compatible is checked at a later time.

	// search for both source and sink block:
	// 1) source block
	if(not impl.exec_m.block_exists(block_source))
	{
		// checked ok. does not leak memory
		throw InvalidBlockNameException(block_source);
	}

	// so we found our source block. now let's see if the given port name was valid
	source_port_it =
		std::find_if
		(
			impl.exec_m[block_source]->get_port_list< OutPort >()->begin(),
			impl.exec_m[block_source]->get_port_list< OutPort >()->end(),
			bind(&OutPort::get_name, ::_1) == port_source
		);

	// unfortunately the given port name was invalid
	if (source_port_it == impl.exec_m[block_source]->get_port_list< OutPort >()->end())
	{
		// checked ok. does not leak memory
		throw InvalidPortNameException(port_source);
	}

	if (source_port_it->send != 0)
	{
		// the ports exists, but has already been connected
		std::cerr << "already connected!" << std::endl;
		return;
	}

	// 2) sink block
	if(not impl.exec_m.block_exists(block_sink))
	{
		// checked ok. does not leak memory
		throw InvalidBlockNameException(block_sink);
	}

	// so we found our sink block. now let's see if the given port name was valid
	sink_port_it =
		std::find_if
		(
			impl.exec_m[block_sink]->get_port_list< InPort >()->begin(),
			impl.exec_m[block_sink]->get_port_list< InPort >()->end(),
			bind(&InPort::get_name, ::_1) == port_sink
		);

	// unfortunately the given port name was invalid
	if (sink_port_it == impl.exec_m[block_sink]->get_port_list< InPort >()->end())
	{
		// checked ok. does not leak memory
		throw InvalidPortNameException(port_sink);
	}

	/*
	*  all provided block- and port names were valid, now let's connect them
	*/

	boost::dynamic_pointer_cast< Source >(impl.exec_m[block_source])->
		connect_calls.push_back(std::make_pair(source_port_it, sink_port_it));

	boost::dynamic_pointer_cast< Source >(impl.exec_m[block_source])->add_connection(block_sink);
}


void System::initialize()
{
	implementation& impl = **this;

	std::vector< block_ptr > start_blocks = impl.exec_m.find_start_blocks();

	std::sort
	(
		start_blocks.begin(),
		start_blocks.end(),
		actions::SourceBlocksToFront()
	);

	for
	(
		std::vector< block_ptr >::iterator source_block = start_blocks.begin();
		source_block != start_blocks.end();
		source_block++
	)
	{
		std::string start_block_name = (*source_block)->get_name_sys();

		if(impl.exec_m.block_is_placed(start_block_name))
		{
			PB_DEBUG_MESSAGE("block '" << start_block_name << "' has already been placed.")
			continue;
		}
		PB_DEBUG_MESSAGE("starting linearization with block '" << start_block_name << "'.")

		impl.exec_m.add_block(start_block_name);

		PB_DEBUG_MESSAGE("linearizing system...")

		impl.linearize(start_block_name);
	}

	if(start_blocks.empty())
	{
		PB_DEBUG_MESSAGE("no start blocks!")
	}

	PB_DEBUG_MESSAGE("setup looks like this:" << std::endl << impl.exec_m)
	PB_DEBUG_MESSAGE("propagating signal attributes and creating signal buffers")

	impl.propagate_signal_attributes();

	PB_DEBUG_MESSAGE("combining execution stages...")

	impl.exec_m.combine_stages();

	PB_DEBUG_MESSAGE("parallelizing...")

	impl.exec_m.parallelize();
	impl.exec_m.init();

	PB_DEBUG_MESSAGE("setup looks like this:" << std::endl << impl.exec_m)
}


void System::wakeup_sys(uint32_t times)
{
	for(uint32_t t=0; t<times; ++t)
	{
		(*this)->exec_m.exec();
	}
}


void System::finalize()
{
	(*this)->exec_m.finalize();
}


SystemImpl::implementation() :
	signal_buffers(),
	signal_buffer_count(0),
	signal_factory(),
	get_buffer_factory(),
	simulation_time(0.0),
	symtab(),
	exec_m()
{
	register_basic_types();
}


void SystemImpl::register_basic_types()
{
//	this macro inserts entries for all Singal types.
//	for integer valued signals, the expansion would look like this:
//	signal_factory.insert(std::make_pair(int32,
//		bind< IntegerSignal* >(new_ptr< IntegerSignal >(), ::_1)));
//	get_buffer_factory.insert(std::make_pair(int32, &get_sig< IntegerSignal >));

#define BOOST_PP_DEF(z, I, _) \
	signal_factory.insert(std::make_pair(TYPE_VALUE(I), \
		bind< SIG_TYPE(I)* >(new_ptr< SIG_TYPE(I) >(), ::_1))); \
	get_buffer_factory.insert(std::make_pair(TYPE_VALUE(I), &get_sig< SIG_TYPE(I) >));

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _);

#undef BOOST_PP_DEF
}


uint32_t SystemImpl::create_signal_buffer(type_t type, uint32_t size)
{
	PB_DEBUG_MESSAGE("      creating signal buffer no. " << signal_buffer_count << ":")
	PB_DEBUG_MESSAGE("        type: " << type << ", size: " << size)

	signal_buffers.push_back(signal_ptr(signal_factory[type](size)));
	return signal_buffer_count++;
}


void SystemImpl::set_buffer_ptrs(OutPort& out, InPort& in, Signal* s)
{
	PB_DEBUG_MESSAGE("        setting buffer aquiration functions for ports '" << out.get_name() << "' and '" << in.get_name() << "'")

	void* (*f)(Signal*) = get_buffer_factory[out.get_type()];

	out.set_signal_ptr( bind(f, s) );
	in.set_signal_ptr( bind(f, s) );
}



void SystemImpl::linearize(const std::string& block_start)
{
	source_ptr src = boost::dynamic_pointer_cast< Source >(exec_m[block_start]);
	if(src)
	std::for_each
	(
		src->get_connections().begin(),
		src->get_connections().end(),
		actions::PlaceBlock(block_start, *this)
	);
}


void SystemImpl::propagate_signal_attributes()
{
	std::for_each
	(
		exec_m.get_stages().begin(),
		exec_m.get_stages().end(),
		actions::SignalAttributePropagation(*this)
	);
}

