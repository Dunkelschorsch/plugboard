#include "exec_stage.hpp"

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

using boost::bind;



ExecutionStage::ExecutionStage() :
	paths_(),
	threading_enabled_(false)
{
}



ExecutionStage::ExecutionStage(Block *b, bool threading_enabled) :
	paths_(),
	threading_enabled_(threading_enabled)
{
	add_block(b);
}



void ExecutionStage::add_block(Block *b)
{
	path_t path_temp;
	path_temp.push_back(b);

	paths_.push_back(path_temp);
}



Block* ExecutionStage::operator[](const std::string& name) const
{
	path_t::const_iterator block_curr;
	for
	(
		stage_t::const_iterator path_curr = paths_.begin();
		path_curr != paths_.end();
		++path_curr
	)
	{
		block_curr = std::find_if
		(
			path_curr->begin(),
			path_curr->end(),
			bind(&Block::get_name_sys, _1) == name
		);
	}
	return *block_curr;
}



bool ExecutionStage::block_is_placed(const std::string& name) const
{
	for
	(
		stage_t::const_iterator path_curr = paths_.begin();
		path_curr != paths_.end();
		++path_curr
	)
	{
		path_t::const_iterator block_curr = std::find_if
		(
			path_curr->begin(),
			path_curr->end(),
			bind(&Block::get_name_sys, _1) == name
		);

		if(block_curr != path_curr->end())
		{
			return true;
		}
	}
	return false;
}



const ExecutionStage::stage_t& ExecutionStage::get_paths() const
{
	return paths_;
}



void ExecutionStage::add_path(const path_t& p)
{
	paths_.push_back(p);
}



ExecutionStage::stage_t& ExecutionStage::get_paths()
{
	return paths_;
}



void ExecutionStage::exec()
{
	if(threading_enabled_)
	{
		boost::thread_group threads;
		uint32_t num_paths = paths_.size();

		assert(num_paths > 1);

		// iterate through paths
		for(uint32_t path_num_curr=0; path_num_curr<num_paths; ++path_num_curr)
		{
			threads.create_thread(bind(&ExecutionStage::exec_path, this, paths_[path_num_curr]));
		}
		threads.join_all();
	}
	else
	{
		std::for_each
		(
			paths_.begin(),
			paths_.end(),
			bind(&ExecutionStage::exec_path, this, _1)
		);
	}
}



void ExecutionStage::exec_path(const path_t& p)
{
	std::for_each
	(
		p.begin(),
		p.end(),
		bind(&Block::process, _1)
	);
}
