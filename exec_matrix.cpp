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

#ifndef NDEBUG
#include <iostream>
#endif

#include "exec_matrix.hpp"
#include "exec_stage.hpp"
#include "block/sink.hpp"
#include "block/source.hpp"

// this one is declared in main.cpp
extern boost::mutex pb_io_mutex;

using boost::bind;
using namespace plugboard;


typedef pimpl< ExecutionMatrix >::implementation ExecutionMatrixImpl;

template< >
struct pimpl< ExecutionMatrix >::implementation
{
	implementation();
	~implementation();

	void add_block(Block * b) ;
	void add_block(Block * b, const std::string& insert_after);
	void add_block(Block * b, uint32_t insert_where);

	void add_stage(ExecutionStage s);

	template< class ActionT >
	void for_each_block(const ActionT);

	typedef std::map< std::string, Block * > block_map_t;

	ExecutionStage::store_t stages_;
	block_map_t blocks_;
};



ExecutionMatrix::ExecutionMatrix() : base() { }


ExecutionMatrix::~ExecutionMatrix() { }


ExecutionMatrixImpl::implementation() :
	stages_(), blocks_()
{ }


ExecutionMatrixImpl::~implementation()
{
	block_map_t::iterator curr_block;
	for(curr_block = blocks_.begin(); curr_block != blocks_.end(); ++curr_block)
	{
		delete curr_block->second;
	}
}


template< typename SequenceT >
bool is_left_terminated(const SequenceT& v)
{
	assert(not v.empty());

	const Sink* sink = dynamic_cast< const Sink* >(v.front());
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

	const Source* source = dynamic_cast< const Source* >(v.back());
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
		if(dynamic_cast< const Source* >(b.second))
		std::cout << "Adding " << b.second->get_name_sys() << " to start blocks." << std::endl;
#endif
		return dynamic_cast< const Source* >(b.second) ?  false : true;
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
			impl.stages_.begin(),
			impl.stages_.end(),
			bind(&ExecutionStage::block_is_placed, _1, name)
		) == impl.stages_.end()
	)
	{
		return false;
	} else
	{
		return true;
	}
}


void ExecutionMatrixImpl::add_block(Block *b)
{
	assert(b != NULL);
	if(dynamic_cast< Source* >(b))
	{
#ifndef NDEBUG
		std::cout << "  This is a source block!" << std::endl;
#endif
		stages_.insert(stages_.begin(), ExecutionStage(b));
	}
	else
	{
		stages_.insert(stages_.end(), ExecutionStage(b));
	}
}


void ExecutionMatrixImpl::add_stage(ExecutionStage s)
{
	stages_.push_back(s);
}


const ExecutionStage::store_t & ExecutionMatrix::get_stages( ) const
{
	return (*this)->stages_;
}


Block * ExecutionMatrix::operator[](const std::string & name) const
{
	implementation::block_map_t::const_iterator block = (*this)->blocks_.find(name);
	if(block == (*this)->blocks_.end())
	{
		return NULL;
	}
	else
	{
		return block->second;
	}
}


void ExecutionMatrixImpl::add_block(Block * b, const std::string & insert_after)
{
	for
	(
		ExecutionStage::store_t::iterator stage_curr = stages_.begin();
		stage_curr != stages_.end();
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
				stages_.insert(++stage_curr, ExecutionStage(b));
				return;
			}
		}
	}
}


void ExecutionMatrix::store_block(Block * b, const std::string & name)
{
	(*this)->blocks_[name] = b;
}


void ExecutionMatrix::add_block(const std::string & name)
{
	(*this)->add_block((*this)->blocks_[name]);
}


void ExecutionMatrix::place_block(const std::string & name, const std::string & insert_after)
{
	(*this)->add_block((*this)->blocks_[name], insert_after);
}


bool ExecutionMatrix::block_exists(const std::string & name) const
{
	implementation::block_map_t::const_iterator it = (*this)->blocks_.find(name);

	if(it != (*this)->blocks_.end())
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

	ExecutionStage::store_t::iterator stage_curr = impl.stages_.begin();
	ExecutionStage::store_t::iterator stage_next = stage_curr;

	do
	{
		while(++stage_next != impl.stages_.end())
		{
#ifndef NDEBUG
			std::cout << *this << std::endl;
			std::cout << "current stage: " << std::endl << *stage_curr << std::endl;
			std::cout << "next stage: " << std::endl << *stage_next << std::endl;
#endif
			if(is_right_terminated(stage_curr->get_paths().back()))
			{
				// nothing more to do here
				++stage_curr;
#ifndef NDEBUG
				std::cout << "stage finished" << std::endl;
#endif
				continue;
			}

			if(is_left_terminated((stage_next)->get_paths().front()))
			{
				// more luck with the next stage?
#ifndef NDEBUG
				std::cout << "stage finished" << std::endl;
#endif
				continue;
			}

			const Block* block_curr = (stage_curr)->get_paths().front().back();
			Block* block_next = (stage_next)->get_paths().front().front();
			assert(block_next != NULL);

			const std::set< std::string > connections_curr
				= dynamic_cast< const Source* >(block_curr)->get_connections();
#ifndef NDEBUG
			std::cout << "checking if block '" << block_curr->get_name_sys() << "' is connected to '";
			std::cout << block_next->get_name_sys() << "' " << std::endl;
#endif
			// if there is a connection move the block on the next stage to the current one
			if(connections_curr.find(block_next->get_name_sys()) != connections_curr.end())
			{
				stage_curr->get_paths().front().push_back(block_next);
				impl.stages_.erase(stage_next);
				stage_next = stage_curr;
			}
		}
		stage_next = ++stage_curr;
	} while(stage_curr != impl.stages_.end());
}


void ExecutionMatrix::parallelize()
{
	implementation& impl = **this;

	ExecutionStage::store_t::iterator stage_curr = impl.stages_.begin();
	ExecutionStage::store_t::iterator stage_next = stage_curr;

	while(++stage_next != impl.stages_.end())
	{
		// iterator through all paths of stage_curr to
		// find a path that inhibits parallelization
		ExecutionStage::stage_t::const_iterator path_it = stage_curr->get_paths().begin();

		do
		{
			const Block* block_curr = path_it->back();
			assert(block_curr != NULL);
#ifndef NDEBUG
			std::cout << "'" << block_curr->get_name_sys() << "' connected to ";
#endif
			const Block* block_next = (stage_next)->get_paths().front().front();
			assert(block_next != NULL);
#ifndef NDEBUG
			std::cout << "'" << block_next->get_name_sys() << "'? " << std::endl;
#endif
			if(not dynamic_cast< const Source* >(block_curr))
			{
#ifndef NDEBUG
				std::cout << "No. Continue checking..." << std::endl;
#endif
				++path_it;
				continue;
			}

			const std::set< std::string > connections_curr
				= dynamic_cast< const Source* >(block_curr)->get_connections();

			if(connections_curr.find(block_next->get_name_sys()) != connections_curr.end())
			{
				// no parallelization possible
#ifndef NDEBUG
				std::cout << "Yes. Proceed to next stage." << std::endl;
#endif
				++stage_curr;
				break;
			}
			else
			{
#ifndef NDEBUG
				std::cout << "No. Continue checking..." << std::endl;
#endif
				++path_it;
			}

		} while(path_it != stage_curr->get_paths().end());

		if(path_it == stage_curr->get_paths().end())
		{
#ifndef NDEBUG
			std::cout << "Moving path..." << std::endl;
#endif
			stage_curr->add_path(stage_next->get_paths().front());
			impl.stages_.erase(stage_next);
#ifndef NDEBUG
			std::cout << *this << std::endl;
#endif
			stage_next = stage_curr;
		}
	}

	std::for_each
	(
		impl.stages_.begin(),
		impl.stages_.end(),
		boost::bind(&ExecutionStage::setup_threading, ::_1)
	);
}


const std::vector< Block * > ExecutionMatrix::find_start_blocks() const
{
	const implementation& impl = **this;

	std::vector< Block * > vv;
	std::vector< std::pair< std::string, Block * > > v;

	std::remove_copy_if
	(
		impl.blocks_.begin(),
		impl.blocks_.end(),
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

	result_type operator()(Block * const b) const
	{
		b->call_initialize();
		b->set_initialized();
	}
};


struct CallFinalize
{
	typedef void result_type;

	result_type operator()(Block * const b) const
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
		ExecutionStage::store_t::iterator stage_it = stages_.begin();
		stage_it != stages_.end();
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
		(*this)->stages_.begin(),
		(*this)->stages_.end(),
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
