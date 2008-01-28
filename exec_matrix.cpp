#include "exec_matrix.hpp"
#include "exec_stage.hpp"

#include <boost/bind.hpp>
#include <algorithm>
#include <iostream>

using boost::bind;


std::ostream& operator<<(std::ostream& out, const ExecutionMatrix& what)
{
	what.print(out);
	return out;
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
		return dynamic_cast< const Source* >(b.second) ? true : false;
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
	if
	(
		std::find_if
		(
			stages_.begin(),
			stages_.end(),
			bind(&ExecutionStage::block_is_placed, _1, name)
		) == stages_.end()
	)
	{
		return false;
	} else
	{
		return true;
	}
}



ExecutionMatrix::ExecutionMatrix() : stages_(), blocks_() { }



ExecutionMatrix::~ExecutionMatrix()
{
	block_map_t::iterator curr_block;
	for(curr_block = blocks_.begin(); curr_block != blocks_.end(); ++curr_block)
	{
		delete curr_block->second;
	}
}



void ExecutionMatrix::add_block(Block *b)
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



void ExecutionMatrix::add_stage(ExecutionStage s)
{
	stages_.push_back(s);
}



const ExecutionStage::store_t& ExecutionMatrix::get_stages() const
{
	return stages_;
}



ExecutionStage::store_t & ExecutionMatrix::get_stages( )
{
	return stages_;
}



Block * ExecutionMatrix::operator[](const std::string & name) const
{
	block_map_t::const_iterator block = blocks_.find(name);
	if(block == blocks_.end())
	{
		return NULL;
	}
	else
	{
		return block->second;
	}
}



void ExecutionMatrix::add_block(Block * b, const std::string & insert_after)
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
			path_curr = stage_curr->paths_.begin();
			path_curr != stage_curr->paths_.end();
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
	blocks_[name] = b;
}



void ExecutionMatrix::add_block(const std::string & name)
{
	add_block(blocks_[name]);
}



void ExecutionMatrix::place_block(const std::string & name, const std::string & insert_after)
{
	add_block(blocks_[name], insert_after);
}



bool ExecutionMatrix::block_exists(const std::string & name) const
{
	block_map_t::const_iterator it = blocks_.find(name);

	if(it != blocks_.end())
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
	ExecutionStage::store_t::iterator stage_curr = stages_.begin();
	ExecutionStage::store_t::iterator stage_next = stage_curr;

	while(++stage_next != stages_.end())
	{
#ifndef NDEBUG
		std::cout << *this << std::endl;
#endif
		if(is_right_terminated(stage_curr->get_paths().back()))
		{
			// nothing more to do here
			++stage_curr;
			continue;
		}

		if(is_left_terminated((stage_next)->get_paths().front()))
		{
			// more luck with the next stage?
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
			stages_.erase(stage_next);
			stage_next = stage_curr;
		}
	}
}



void ExecutionMatrix::parallelize()
{
	ExecutionStage::store_t::iterator stage_curr = stages_.begin();
	ExecutionStage::store_t::iterator stage_next = stage_curr;

	while(++stage_next != stages_.end())
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
			stages_.erase(stage_next);
#ifndef NDEBUG
			std::cout << *this << std::endl;
#endif
			stage_next = stage_curr;
		}
	}
}



std::vector< Block * > ExecutionMatrix::find_start_blocks()
{
	std::vector< Block * > vv;

	std::vector< block_map_t::value_type > v
	(
		std::find_if
		(
			blocks_.begin(),
			blocks_.end(),
			FindStartBlock()
		),
		blocks_.end()
	);

	std::for_each
	(
		v.begin(),
		v.end(),
		pair_unwrapper(vv)
	);

	return vv;
}



void ExecutionMatrix::init()
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
		bind(&Block::initialize, _1)
	);
}



void ExecutionMatrix::exec()
{
	std::for_each
	(
		stages_.begin(),
		stages_.end(),
		bind(&ExecutionStage::exec, _1)
	);
}



void ExecutionMatrix::print(std::ostream& out) const
{
#ifndef NDEBUG
		out << std::endl << "-------------" << std::endl;
#endif
		uint32_t j=0;
		for
		(
			ExecutionStage::store_t::const_iterator stage_it = get_stages().begin();
			stage_it != get_stages().end();
			++stage_it
		)
		{
			out << "Stage: " << j++ << std::endl;
			out << *stage_it;
		}
#ifndef NDEBUG
		out << "-------------" << std::endl;
#endif
}
