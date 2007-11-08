#include "exec_matrix.hpp"
#include "exec_stage.hpp"

#include <boost/bind.hpp>
#include <algorithm>

#ifndef NDEBUG
#include <iostream>
#endif

using boost::bind;


namespace
{
	template< typename SequenceT >
	bool is_left_terminated(const SequenceT& v)
	{
		assert(not v.empty());
		return v.front()->get_num_input_ports() > 1 || v.front()->get_num_input_ports() == 0;
	}


	template< typename SequenceT >
	bool is_right_terminated(const SequenceT& v)
	{
		assert(not v.empty());
		return v.back()->get_num_output_ports() > 1 || v.back()->get_num_output_ports() == 0;
	}


	struct FindStartBlock
	{
		template< typename PairT >
		bool operator()(const PairT& b) const
		{
			if(b.second->get_num_input_ports() == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		typedef bool result_type;
	};


	template< class ContainerT >
	struct UnwrapPair
	{
		UnwrapPair(ContainerT& c) : vec_(c) { }

		template< typename PairT >
		void operator()(const PairT& b)
		{
 			vec_.push_back(b.second);
		}

		ContainerT& vec_;

		typedef void result_type;
	};


	// automatically deduce the type of the container to save typing
	template< typename ContainerT >
	inline UnwrapPair< ContainerT > pair_unwrapper(ContainerT& c)
	{
		return UnwrapPair< ContainerT >(c);
	}
}



bool ExecutionMatrix::block_is_placed(const std::string& name) const
{
	for
	(
		ExecutionStage::store_t::const_iterator stage_it = stages_.begin();
		stage_it != stages_.end();
		++stage_it
	)
	{
		if(stage_it->block_is_placed(name))
		{
			return true;
		}
	}
	return false;
}



ExecutionMatrix::ExecutionMatrix() : stages_(), blocks_()
{
}



ExecutionMatrix::~ExecutionMatrix()
{
	block_map_t::iterator curr_block;
	for(curr_block = blocks_.begin(); curr_block != blocks_.end(); ++curr_block)
	{
		delete curr_block->second;
	}
}



template< typename IteratorT >
void ExecutionMatrix::swap_stages(IteratorT s1, IteratorT s2)
{
	std::swap(s1, s2);
}



template< >
void ExecutionMatrix::swap_stages< int >(int s1, int s2)
{
	std::swap(stages_[s1], stages_[s2]);
}



void ExecutionMatrix::add_block(Block *b)
{
	assert(b != NULL);
	if(b->get_num_input_ports() == 0)
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



void ExecutionMatrix::add_block(Block * b, uint32_t insert_where)
{
	assert(b != NULL);
	stages_.insert(stages_.begin()+insert_where+1, ExecutionStage(b));
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
				stages_.insert(stage_curr+1, ExecutionStage(b));
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
	ExecutionStage::store_t::iterator stage_curr;
	std::set< std::string >::const_iterator conn_it;
	
	for(stage_curr = stages_.begin(); stage_curr != stages_.end(); stage_curr++)
	{
		// that is the rightmost block of the first (and only) path in the currently examined execution stage
		Block* block_curr = (stage_curr)->get_paths().front().back();
		assert(block_curr != NULL);

#ifndef NDEBUG
		std::cout << "checking if block '" << block_curr->get_name_sys() << "' is connected to its successors." << std::endl;
#endif

		if(stage_curr+1 == stages_.end())
		{
			break;
		}
		// that is the leftmost block of the first (and only) path in the next execution stage
		Block* block_succ = (stage_curr+1)->get_paths().front().front();
		assert(block_succ != NULL);

		// maybe this needs some explanation ... later
		conn_it = block_curr->get_connections().find(block_succ->get_name_sys());
		while
		(
			conn_it != block_curr->get_connections().end() &&
			!is_right_terminated(stage_curr->get_paths().front()) &&
			!is_left_terminated((stage_curr+1)->get_paths().front())
		)
		{
#ifndef NDEBUG
			std::cout << "moving: " << block_succ->get_name_sys() << std::endl;
#endif
			stage_curr->get_paths().front().push_back(block_succ);
			stages_.erase(stage_curr+1);
#ifndef NDEBUG
			std::cout << *this;
#endif
			if (stage_curr+1 == stages_.end())
			{
				break;
			}
			else if (stage_curr+2 != stages_.end())
			{
				stage_curr++;
			}

			block_curr = (stage_curr)->get_paths().front().back();
			block_succ = (stage_curr+1)->get_paths().front().front();

			assert(block_succ != block_curr);

			conn_it = block_curr->get_connections().find(block_succ->get_name_sys());
		}
	}
}



void ExecutionMatrix::parallelize()
{
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
		std::cout << "checking against " << block_succ->get_name_sys() << std::endl;
		std::cout << "parallelize(): proceeding to next stage" << std::endl;
#endif
		while(!proceed_to_next_stage)
		{
			proceed_to_next_stage = false;

			for(path_curr = (stage_curr)->get_paths().begin(); path_curr != (stage_curr)->get_paths().end(); path_curr++)
			{
#ifndef NDEBUG
				std::cout << "Obacht: " << std::endl << *(stage_curr+1);
#endif
				// that is the rightmost block of the currently investigated path in the currently examined execution stage
				block_curr = path_curr->back();
#ifndef NDEBUG
				std::cout << "parallelize(): '";
				std::cout << block_curr->get_name_sys() << "' connected to '";
				std::cout << block_succ->get_name_sys() << "'?";
#endif
				assert(block_curr != block_succ);
				conn_it = block_curr->get_connections().find(block_succ->get_name_sys());

				// if block_curr and block_succ ARE connected ...
				if(conn_it != block_curr->get_connections().end())
				{
#ifndef NDEBUG
					std::cout << " ... apparently" << std::endl;
#endif
					parallelizeable = false;
					break;
				}
			}

			if(parallelizeable)
			{
#ifndef NDEBUG
				std::cout << std::endl << "parallelize(): moving '" << block_succ->get_name_sys() << "'" << std::endl;
#endif
				stage_curr->threading_enabled_ = true;
				stage_curr->add_path((stage_curr+1)->get_paths().front());
				stages_.erase(stage_curr+1);
#ifndef NDEBUG
				std::cout << *this;
#endif
				if(stage_curr+1 == stages_.end() || stage_curr+2 == stages_.end())
				{
#ifndef NDEBUG
					std::cout << "parallelize(): at last stage" << std::endl;
#endif
					return;
				}
				// neccessary to compensate for the deleted stage
				stage_curr++;
				// that is the leftmost block of the first (and only) path in the next execution stage
				block_succ = (stage_curr+1)->get_paths().front().front();
#ifndef NDEBUG
				std::cout << "next stage: " << std::endl << *(stage_curr+1);
				std::cout << "parallelize(): next reference block: " << block_succ->get_name_sys() << std::endl;
#endif
			}
			else
			{
				proceed_to_next_stage = true;
			}
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
	for(uint32_t i=0; i<stages_.size(); ++i)
		// iterate over all paths
		for(uint32_t j=0; j<stages_[i].paths_.size(); ++j)
			// iterate over all blocks
			std::for_each
			(
				stages_[i].paths_[j].begin(),
				stages_[i].paths_[j].end(),
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
