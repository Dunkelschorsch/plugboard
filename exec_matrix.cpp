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

#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <algorithm>

#include "exec_matrix.hpp"
#include "exec_stage.hpp"
#include "block/sink.hpp"
#include "block/source.hpp"

// this one is declared in main.cpp
extern boost::mutex pb_io_mutex;
#define PB_DEBUG_MESSAGE_COLOUR \033[01;33m
#define PB_DEBUG_MESSAGE_SOURCE ExecutionMatrix

#include "colour_debug.hpp"

using boost::bind;
using namespace plugboard;


typedef pimpl< ExecutionMatrix >::implementation ExecutionMatrixImpl;

template< >
struct pimpl< ExecutionMatrix >::implementation
{
	implementation();

	void add_block(block_ptr b) ;
	void add_block(block_ptr b, const std::string& insert_after);
	void add_block(block_ptr b, uint32_t insert_where);

	void add_stage(ExecutionStage s);

	template< class ActionT >
	void for_each_block(const ActionT);

	typedef std::map< std::string, block_ptr > block_map_t;

	ExecutionStage::store_t stages;
	block_map_t blocks;
};



ExecutionMatrix::ExecutionMatrix() : base() { }


ExecutionMatrix::~ExecutionMatrix() { }


ExecutionMatrixImpl::implementation() :
	stages(), blocks()
{ }


template< typename SequenceT >
bool is_left_terminated(const SequenceT& v)
{
	assert(not v.empty());

	const_sink_ptr sink = boost::dynamic_pointer_cast< const Sink >(v.front());
	if(sink)
	{
		if(sink->get_num_input_ports() > 1)
			return true;
		else 
			return false;
	} else
		return true;
}


template< typename SequenceT >
bool is_right_terminated(const SequenceT& v)
{
	assert(not v.empty());

	const_source_ptr source = boost::dynamic_pointer_cast< const Source >(v.back());
	if(source)
	{
		if(source->get_num_output_ports() > 1)
			return true;
		else 
			return false;
	} else
		return true;
}


struct FindStartBlock
{
	typedef bool result_type;

	template< typename PairT >
	result_type operator()(const PairT& b) const
	{
#ifndef NDEBUG
		if(boost::dynamic_pointer_cast< const Source >(b.second))
		PB_DEBUG_MESSAGE("Adding " << b.second->get_name_sys() << " to start blocks.")
#endif
		return boost::dynamic_pointer_cast< const Source >(b.second) ?  false : true;
	}

};


template< class ContainerT >
struct UnwrapPair
{
	typedef void result_type;

	UnwrapPair(ContainerT& c) : vec_(c) { }

	template< typename PairT >
	result_type operator()(const PairT& b)
	{
		vec_.push_back(b.second);
	}

	ContainerT& vec_;
};


template< typename ContainerT >
inline UnwrapPair< ContainerT > pair_unwrapper(ContainerT& c)
{
	return UnwrapPair< ContainerT >(c);
}


bool ExecutionMatrix::block_is_placed(const std::string& name) const
{
	const implementation& impl = **this;
	if
	(
		std::find_if
		(
			impl.stages.begin(),
			impl.stages.end(),
			bind(&ExecutionStage::block_is_placed, _1, name)
		) == impl.stages.end()
	)
	{
		return false;
	} else
	{
		return true;
	}
}


void ExecutionMatrixImpl::add_block(block_ptr b)
{
	assert(b != NULL);
	if(boost::dynamic_pointer_cast< const Source >(b))
	{
		PB_DEBUG_MESSAGE("This is a source block!")

		stages.insert(stages.begin(), ExecutionStage(b));
	}
	else
	{
		stages.insert(stages.end(), ExecutionStage(b));
	}
}


void ExecutionMatrixImpl::add_stage(ExecutionStage s)
{
	stages.push_back(s);
}


const ExecutionStage::store_t & ExecutionMatrix::get_stages( ) const
{
	return (*this)->stages;
}


block_ptr ExecutionMatrix::operator[](const std::string & name) const
{
	implementation::block_map_t::const_iterator block = (*this)->blocks.find(name);
	if(block == (*this)->blocks.end())
	{
		return block_ptr();
	}
	else
	{
		return block->second;
	}
}


void ExecutionMatrixImpl::add_block(block_ptr b, const std::string & insert_after)
{
	for
	(
		ExecutionStage::store_t::iterator stage_curr = stages.begin();
		stage_curr != stages.end();
		++stage_curr
	)
	{
		ExecutionStage::path_t::const_iterator block_curr;
		ExecutionStage::stage_t::const_iterator path_curr;
		for
		(
			path_curr = stage_curr->get_paths().begin();
			path_curr != stage_curr->get_paths().end();
			++path_curr
		)
		{
			block_curr = std::find_if
			(
				path_curr->begin(),
				path_curr->end(),
				bind(&Block::get_name_sys, _1) == insert_after
			);

			if(block_curr != path_curr->end())
			{
				stages.insert(++stage_curr, ExecutionStage(b));
				return;
			}
		}
	}
}


void ExecutionMatrix::store_block(block_ptr b, const std::string & name)
{
	(*this)->blocks[name] = b;
}


void ExecutionMatrix::add_block(const std::string & name)
{
	(*this)->add_block((*this)->blocks[name]);
}


void ExecutionMatrix::place_block(const std::string & name, const std::string & insert_after)
{
	(*this)->add_block((*this)->blocks[name], insert_after);
}


bool ExecutionMatrix::block_exists(const std::string & name) const
{
	implementation::block_map_t::const_iterator it = (*this)->blocks.find(name);

	if(it != (*this)->blocks.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}


void ExecutionMatrix::combine_stages()
{
	implementation& impl = **this;

	ExecutionStage::store_t::iterator stage_curr = impl.stages.begin();
	ExecutionStage::store_t::iterator stage_next = stage_curr;

	do
	{
		while(++stage_next != impl.stages.end())
		{
			PB_DEBUG_MESSAGE("setup looks like this:" << std::endl << *this)
			PB_DEBUG_MESSAGE("current stage: " << std::endl << *stage_curr)
			PB_DEBUG_MESSAGE("next stage: " << std::endl << *stage_next)

			if(is_right_terminated(stage_curr->get_paths().back()))
			{
				// nothing more to do here
				++stage_curr;

				PB_DEBUG_MESSAGE("stage finished")

				continue;
			}

			if(is_left_terminated((stage_next)->get_paths().front()))
			{
				// more luck with the next stage?
				PB_DEBUG_MESSAGE("stage finished")

				continue;
			}

			const block_ptr block_curr = (stage_curr)->get_paths().front().back();
			block_ptr block_next = (stage_next)->get_paths().front().front();
			assert(block_next != block_ptr());

			const std::set< std::string > connections_curr
				= boost::dynamic_pointer_cast< const Source >(block_curr)->get_connections();

			PB_DEBUG_MESSAGE("checking if block '" << block_curr->get_name_sys() << "' is connected to '" <<
				block_next->get_name_sys() << "' ")

			// if there is a connection move the block on the next stage to the current one
			if(connections_curr.find(block_next->get_name_sys()) != connections_curr.end())
			{
				stage_curr->get_paths().front().push_back(block_next);
				impl.stages.erase(stage_next);
				stage_next = stage_curr;
			}
		}
		stage_next = ++stage_curr;
	} while(stage_curr != impl.stages.end());
}


void ExecutionMatrix::parallelize()
{
	implementation& impl = **this;

	ExecutionStage::store_t::iterator stage_curr = impl.stages.begin();
	ExecutionStage::store_t::iterator stage_next = stage_curr;

	while(++stage_next != impl.stages.end())
	{
		// iterator through all paths of stage_curr to
		// find a path that inhibits parallelization
		ExecutionStage::stage_t::const_iterator path_it = stage_curr->get_paths().begin();

		do
		{
			const block_ptr block_curr = path_it->back();
			assert(block_curr != block_ptr());

			PB_DEBUG_MESSAGE("'" << block_curr->get_name_sys() << "' connected to ...")

			const block_ptr block_next = (stage_next)->get_paths().front().front();
			assert(block_next != block_ptr());

			PB_DEBUG_MESSAGE("... '" << block_next->get_name_sys() << "'? ")

			if(not boost::dynamic_pointer_cast< const Source >(block_curr))
			{
				PB_DEBUG_MESSAGE("No. Continue checking...")
				++path_it;
				continue;
			}

			const std::set< std::string > connections_curr
				= boost::dynamic_pointer_cast< const Source >(block_curr)->get_connections();

			if(connections_curr.find(block_next->get_name_sys()) != connections_curr.end())
			{
				// no parallelization possible
				PB_DEBUG_MESSAGE("Yes. Proceed to next stage.")
				++stage_curr;
				break;
			}
			else
			{
				PB_DEBUG_MESSAGE("No. Continue checking...")
				++path_it;
			}

		} while(path_it != stage_curr->get_paths().end());

		if(path_it == stage_curr->get_paths().end())
		{
			PB_DEBUG_MESSAGE("Moving path...")

			stage_curr->add_path(stage_next->get_paths().front());
			impl.stages.erase(stage_next);

			PB_DEBUG_MESSAGE("setup looks like this:" << std::endl << *this)

			stage_next = stage_curr;
		}
	}

	std::for_each
	(
		impl.stages.begin(),
		impl.stages.end(),
		boost::bind(&ExecutionStage::setup_threading, ::_1)
	);
}


const std::vector< block_ptr > ExecutionMatrix::find_start_blocks() const
{
	const implementation& impl = **this;

	std::vector< block_ptr > vv;
	std::vector< std::pair< std::string, block_ptr > > v;

	std::remove_copy_if
	(
		impl.blocks.begin(),
		impl.blocks.end(),
		std::back_inserter(v),
		FindStartBlock()
	);

	std::for_each
	(
		v.begin(),
		v.end(),
		pair_unwrapper(vv)
	);

	return vv;
}


struct CallInit
{
	typedef void result_type;

	result_type operator()(block_ptr const b) const
	{
		b->call_initialize();
		b->set_initialized();
	}
};


struct CallFinalize
{
	typedef void result_type;

	result_type operator()(block_ptr const b) const
	{
		b->call_finalize();
	}
};


template< class ActionT >
void ExecutionMatrixImpl::for_each_block(const ActionT f)
{
	// iterate over all stages
	for
	(
		ExecutionStage::store_t::iterator stage_it = stages.begin();
		stage_it != stages.end();
		++stage_it
	)
	// iterate over all paths in current stage
	for
	(
		ExecutionStage::stage_t::iterator path_it = stage_it->get_paths().begin();
		path_it != stage_it->get_paths().end();
		++path_it
	)
	// iterate over all blocks in current path
	std::for_each
	(
		path_it->begin(),
		path_it->end(),
		f
	);
}


void ExecutionMatrix::init()
{
	(*this)->for_each_block(CallInit());
	return;
}


void ExecutionMatrix::exec()
{
	std::for_each
	(
		(*this)->stages.begin(),
		(*this)->stages.end(),
		bind(&ExecutionStage::exec, _1)
	);
}


void ExecutionMatrix::finalize()
{
	(*this)->for_each_block(CallFinalize());
	return;
}


void ExecutionMatrix::print(std::ostream& out) const
{
#ifndef NDEBUG
	{
		const boost::mutex::scoped_lock lock(pb_io_mutex);
		out << std::endl << "-------------" << std::endl;
	}
#endif
	uint32_t j=0;
	for
	(
		ExecutionStage::store_t::const_iterator stage_it = get_stages().begin();
		stage_it != get_stages().end();
		++stage_it
	)
	{
		{
			const boost::mutex::scoped_lock lock(pb_io_mutex);
			out << "Stage: " << j++ << std::endl;
		}
		out << *stage_it;
	}
#ifndef NDEBUG
	{
		const boost::mutex::scoped_lock lock(pb_io_mutex);
		out << std::endl << "-------------" << std::endl;
	}
#endif
}


namespace plugboard
{
	std::ostream& operator<<(std::ostream& out, const ExecutionMatrix& what)
	{
		what.print(out);
		return out;
	}
}
