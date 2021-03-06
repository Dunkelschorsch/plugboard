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

#include "exec_stage.hpp"
#include "environment.hpp"
#include "thread_mgm.hpp"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#define PB_DEBUG_MESSAGE_COLOUR \033[01;33m
#define PB_DEBUG_MESSAGE_SOURCE ExecutionStage

#include "colour_debug.hpp"


using boost::bind;
using namespace plugboard;

// this one is declared in main.cpp
extern boost::mutex pb_io_mutex;

boost::mutex worker_creation_mutex;


typedef pimpl< ExecutionStage >::implementation ExecutionStageImpl;


template< >
struct pimpl< ExecutionStage >::implementation
{
	implementation();
	implementation(block_ptr const b, bool threading_enabled);

	void add_block_impl(block_ptr const b);

	plugboard::ExecutionStage::stage_t paths;
	bool threading_enabled, calibrating, test_multi;

	boost::posix_time::time_duration time_single_threaded, time_multi_threaded;

	static const uint8_t test_runs = 30;
	uint64_t execution_count;

	boost::thread_group threads;

	Boss boss;
};



plugboard::ExecutionStage::ExecutionStage() :
	base()
{ }



plugboard::ExecutionStage::ExecutionStage(block_ptr const b, bool threading_enabled) :
	base(b, threading_enabled)
{ }



ExecutionStageImpl::implementation() :
	paths(),
	threading_enabled(false),
	calibrating(true),
	test_multi(false),
	time_single_threaded(0,0,0,0),
	time_multi_threaded(0,0,0,0),
	execution_count(0)
{ }


ExecutionStageImpl::implementation(block_ptr const b, bool threading_enabled) :
	threading_enabled(threading_enabled),
	calibrating(true),
	test_multi(false),
	time_single_threaded(0,0,0,0),
	time_multi_threaded(0,0,0,0),
	execution_count(0)
{
	add_block_impl(b);
}


void ExecutionStageImpl::add_block_impl(block_ptr const b)
{
	plugboard::ExecutionStage::path_t path_temp;
	path_temp.push_back(b);
	paths.push_back(path_temp);
}


void plugboard::ExecutionStage::add_block(block_ptr const b)
{
	(*this)->add_block_impl(b);
}


block_ptr plugboard::ExecutionStage::operator[](const std::string& name) const
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


bool plugboard::ExecutionStage::block_is_placed(const std::string& name) const
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


const plugboard::ExecutionStage::stage_t& plugboard::ExecutionStage::get_paths() const
{
	return (*this)->paths;
}


void plugboard::ExecutionStage::add_path(const path_t& p)
{
	(*this)->paths.push_back(p);

	if((get_paths().size() > 1) &&
		boost::any_cast< bool >(Environment::instance().get("threading")))
	{
		(*this)->threading_enabled = true;
	}
}


plugboard::ExecutionStage::stage_t& plugboard::ExecutionStage::get_paths()
{
	return (*this)->paths;
}


void plugboard::ExecutionStage::exec()
{
	using namespace boost::posix_time;
	implementation& impl = **this;

	if(impl.calibrating && impl.threading_enabled)
	{
		if(impl.test_multi)
		{
			PB_DEBUG_MESSAGE("measuring testing multi-threaded execution time")
			ptime start(microsec_clock::universal_time());

			impl.boss.continue_all();
			impl.boss.sync_post_process();

			ptime end(microsec_clock::universal_time());
			impl.time_multi_threaded += (end - start);
		} else
		{
			PB_DEBUG_MESSAGE("measuring single-threading execution time")
			ptime start(microsec_clock::universal_time());

			std::for_each
			(
				get_paths().begin(),
				get_paths().end(),
				bind(&plugboard::ExecutionStage::exec_path, this, _1)
			);

			ptime end(microsec_clock::universal_time());
			impl.time_single_threaded += (end - start);

		}

		impl.test_multi ^= true;
		if(impl.execution_count == (impl.test_runs | 1u))
		{
			impl.calibrating = false;
			impl.threading_enabled = impl.time_multi_threaded < impl.time_single_threaded;

			PB_DEBUG_MESSAGE("Accumulated single-threaded execution time: " <<
				impl.time_single_threaded.total_microseconds() << " ms")
			PB_DEBUG_MESSAGE("Accumulated multi-threaded execution time:  " <<
				impl.time_multi_threaded.total_microseconds() << " ms")
			PB_DEBUG_MESSAGE("multithreading is " << (impl.threading_enabled ? "en" : "dis") << "abled")
			PB_DEBUG_MESSAGE("estimated speedup: "  <<
				static_cast<double>(impl.time_single_threaded.fractional_seconds())/
				static_cast<double>(impl.time_multi_threaded.fractional_seconds())
			)
		}
	}

	if(impl.threading_enabled)
	{
		impl.boss.continue_all();
		impl.boss.sync_post_process();
	}
	// execute all paths sequentially
	else
	{
		std::for_each
		(
			get_paths().begin(),
			get_paths().end(),
			bind(&plugboard::ExecutionStage::exec_path, this, _1)
		);
	}

	impl.execution_count++;
}


void plugboard::ExecutionStage::exec_path(const path_t& p)
{
	std::for_each
	(
		p.begin(),
		p.end(),
		bind(&Block::call_process, _1)
	);
}


void plugboard::ExecutionStage::print(std::ostream& out) const
{
	uint32_t path_num=0;
	for
	(
		plugboard::ExecutionStage::stage_t::const_iterator path_it = get_paths().begin();
		path_it != get_paths().end();
		++path_it
	)
	{
		{
			const boost::mutex::scoped_lock lock(pb_io_mutex);
			out << "  Path: " << path_num++ << std::endl;
		}

		for
		(
			plugboard::ExecutionStage::path_t::const_iterator block_it = path_it->begin();
			block_it != path_it->end();
			++block_it
		)
		{
			{
				const boost::mutex::scoped_lock lock(pb_io_mutex);
				out << "   " << (*block_it)->get_name_sys();
			}
		}

		{
			const boost::mutex::scoped_lock lock(pb_io_mutex);
			out << std::endl;
		}
	}
}


void plugboard::ExecutionStage::setup_threading()
{
	implementation& impl = **this;

	const boost::mutex::scoped_lock lock(worker_creation_mutex);

	if(not impl.threading_enabled)
	{
		return;
	}

	impl.boss = Boss(impl.threads, this);
	
	std::for_each
	(
		get_paths().begin(),
		get_paths().end(),
		boost::bind(&Boss::new_thread_from_path, boost::ref(impl.boss), _1)
	);
}


plugboard::ExecutionStage::~ExecutionStage()
{
	implementation& impl = **this;

	if(impl.threading_enabled)
	{
		PB_DEBUG_MESSAGE_LOCKED("shutting down threads")
		impl.boss.shutdown_now();
	}
}


namespace plugboard
{
	std::ostream& operator<<(std::ostream& out, const ExecutionStage& what)
	{
		what.print(out);
		return out;
	}
}
