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
	ExecutionStage::store_t::iterator stage_next;

	while(1)
	{
		stage_next = stage_curr;
		++stage_next;
#ifndef NDEBUG
		std::cout << *this << std::endl;
#endif
		if(stage_next == stages_.end())
			break;

		if(is_right_terminated(stage_curr->get_paths().back())
			or is_left_terminated((stage_next)->get_paths().front()))
		{
			// nothing more to do here
			++stage_curr;
			continue;
		}

		Block* block_curr = (stage_curr)->get_paths().front().back();
		Block* block_next = (stage_next)->get_paths().front().front();
		assert(block_next != NULL);

		const std::set< std::string > connections_curr
			= dynamic_cast< Source* >(block_curr)->get_connections();
#ifndef NDEBUG
		std::cout << "checking if block '" << block_curr->get_name_sys() << "' is connected to '";
		std::cout << block_next->get_name_sys() << "' " << std::endl;
#endif
		// if there is a connection move the block on the next stage to the current one
		if(connections_curr.find(block_next->get_name_sys()) != connections_curr.end())
		{
			stage_curr->get_paths().front().push_back(block_next);
			stages_.erase(stage_next);
		} else
		{
			++stage_curr;
		}
	}
}



void ExecutionMatrix::parallelize()
{
#if 0
	ExecutionStage::store_t::iterator stage_curr;
	ExecutionStage::stage_t::iterator path_curr;
	std::set< std::string >::const_iterator conn_it;

	Block *block_curr, *block_succ;

	bool parallelizeable, proceed_to_next_stage;

	for(stage_curr = stages_.begin(); stage_curr != stages_.end()-1; stage_curr++)
	{
		proceed_to_next_stage = false;
		parallelizeable = true;

		// that is the leftmost block of the first (and only) path in the next execution stage
		block_succ = (stage_curr+1)->get_paths().front().front();
#ifndef NDEBUG
		std::cout << "checking against block '" << block_succ->get_name_sys() << "'" << std::endl;
#endif
		while(!proceed_to_next_stage)
		{
			proceed_to_next_stage = false;

			for(path_curr = (stage_curr)->get_paths().begin(); path_curr != (stage_curr)->get_paths().end(); path_curr++)
			{
				// that is the rightmost block of the currently investigated path in the currently examined execution stage
				block_curr = path_curr->back();
#ifndef NDEBUG
				std::cout << "parallelize(): '";
				std::cout << block_curr->get_name_sys() << "' connected to '";
				std::cout << block_succ->get_name_sys() << "'?";
#endif
				if(block_curr == block_succ)
				{
					return;
				}
				conn_it = dynamic_cast< Source* >(block_curr)->get_connections().find(block_succ->get_name_sys());

				// if block_curr and block_succ ARE connected ...
				if(conn_it != dynamic_cast< Source* >(block_curr)->get_connections().end())
				{
#ifndef NDEBUG
					std::cout << " Yes.";
#endif
					parallelizeable = false;
					break;
				}
#ifndef NDEBUG
				else
				{
					std::cout << " No.";
				}
#endif
			}

			if(parallelizeable)
			{
#ifndef NDEBUG
				std::cout << std::endl << "parallelize(): moving stage: " << std::endl << *(stage_curr+1);
#endif
				stage_curr->threading_enabled_ = true;
				stage_curr->add_path((stage_curr+1)->get_paths().front());
				stages_.erase(stage_curr+1);
#ifndef NDEBUG
				std::cout << *this;
#endif
				if(get_stages().size() == 1)
				{
#ifndef NDEBUG
					std::cout << "parallelize(): system now only consists of a single stage" << std::endl;
#endif
					return;
				}

				if(stage_curr+1 == stages_.end() || stage_curr+2 == stages_.end())
				{
#ifndef NDEBUG
					std::cout << "parallelize(): at last stage" << std::endl;
#endif
					continue;
				}
				// neccessary to compensate for the deleted stage
				stage_curr++;
				// that is the leftmost block of the first (and only) path in the next execution stage
				block_succ = (stage_curr+1)->get_paths().front().front();
#ifndef NDEBUG
				std::cout << "next stage: " << std::endl << *(stage_curr+1) << std::endl;
				std::cout << "parallelize(): next reference block: " << block_succ->get_name_sys() << std::endl;
#endif
			}
			else
			{
				proceed_to_next_stage = true;
			}
		}
	}
#endif
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
		out << "-------------" << std::endl << std::endl;
#endif
}
