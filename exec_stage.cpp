#include "exec_stage.hpp"
#include "environment.hpp"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/pool/object_pool.hpp>

using boost::bind;


typedef pimpl< ExecutionStage >::implementation ExecutionStageImpl;


template< >
struct pimpl< ExecutionStage >::implementation
{
	implementation();
	implementation(Block * const b, bool threading_enabled);

	void add_block_impl(Block * const b);

	ExecutionStage::stage_t paths_;
	bool threading_enabled_;
	std::vector< boost::thread* > thread_group_;
};


std::ostream& operator<<(std::ostream& out, const ExecutionStage& what)
{
	what.print(out);
	return out;
}



ExecutionStage::ExecutionStage() :
	base()
{ }



ExecutionStage::ExecutionStage(Block * const b, bool threading_enabled) :
	base(b, threading_enabled)
{ }



ExecutionStageImpl::implementation() :
	paths_(),
	threading_enabled_(false)
{ }




ExecutionStageImpl::implementation(Block * const b, bool threading_enabled) :
	threading_enabled_(threading_enabled)
{
	add_block_impl(b);
}



void ExecutionStageImpl::add_block_impl(Block * const b)
{
	ExecutionStage::path_t path_temp;
	path_temp.push_back(b);
	paths_.push_back(path_temp);
}



void ExecutionStage::add_block(Block * const b)
{
	(*this)->add_block_impl(b);
}



Block* ExecutionStage::operator[](const std::string& name) const
{
	path_t::const_iterator block_curr;
	stage_t::const_iterator path_curr;
	for
	(
		path_curr = get_paths().begin();
		path_curr != get_paths().end();
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
	assert(block_curr != path_curr->end());
	return *block_curr;
}



bool ExecutionStage::block_is_placed(const std::string& name) const
{
	for
	(
		stage_t::const_iterator path_curr = get_paths().begin();
		path_curr != get_paths().end();
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
	return (*this)->paths_;
}



void ExecutionStage::add_path(const path_t& p)
{
	if((get_paths().size() > 1) &&
		boost::any_cast< bool >(Environment::instance().get("threading")))
	{
		(*this)->threading_enabled_ = true;
	}

	(*this)->paths_.push_back(p);
}



ExecutionStage::stage_t& ExecutionStage::get_paths()
{
	return (*this)->paths_;
}



void ExecutionStage::exec()
{
	implementation& impl = **this;

	if(impl.threading_enabled_)
	{
		assert(get_paths().size() > 1);
		boost::object_pool< boost::thread > thread_pool;

		// iterate through paths
		stage_t::const_iterator path_it;
		stage_t::const_iterator paths_end = get_paths().end();
		for
		(
			path_it = get_paths().begin();
			path_it != paths_end;
			++path_it
		)
		{
			boost::thread* t = thread_pool.construct(bind(&ExecutionStage::exec_path, this, *path_it));
			impl.thread_group_.push_back(t);
		}
		// wait for all threads to finish
		std::for_each
		(
			impl.thread_group_.begin(),
			impl.thread_group_.end(),
			bind(&boost::thread::join, _1)
		);
 		impl.thread_group_.clear();
	}
	// execute all paths sequentially
	else
	{
		std::for_each
		(
			get_paths().begin(),
			get_paths().end(),
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
		bind(&Block::call_process, _1)
	);
}



void ExecutionStage::print(std::ostream& out) const
{
	uint32_t path_num=0;
	for
	(
		ExecutionStage::stage_t::const_iterator path_it = get_paths().begin();
		path_it != get_paths().end();
		++path_it
	)
	{
		out << "  Path: " << path_num++ << std::endl;
		for
		(
			ExecutionStage::path_t::const_iterator block_it = path_it->begin();
			block_it != path_it->end();
			++block_it
		)
		{
			out << "   " << (*block_it)->get_name_sys();
		}
		out << std::endl;

	}
}
