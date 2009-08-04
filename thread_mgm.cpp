// -*- C++ -*-

#include "thread_mgm.hpp"
#include <tr1/functional>

extern boost::mutex pb_io_mutex;
#define PB_DEBUG_MESSAGE_COLOUR \033[01;33m
#define PB_DEBUG_MESSAGE_SOURCE ThreadMgm

#include "colour_debug.hpp"


namespace plugboard
{
	uint32_t Boss::id = 0;


	Boss::Boss() : tt_(NULL) { }


	Boss::Boss(boost::thread_group& tt, ExecutionStage* stage) : tt_(&tt), stage_(stage), shutdown_(false)
	{
		PB_DEBUG_MESSAGE("creating rendezvous points for " << stage_->get_paths().size() << " threads.")

		before_process = new boost::barrier(1 + stage_->get_paths().size());
		after_process = new boost::barrier(1 + stage_->get_paths().size());

		PB_DEBUG_MESSAGE("constructed manager object.")
	}


	void Boss::new_thread_from_path(const ExecutionStage::path_t &path)
	{
		PB_DEBUG_MESSAGE("adding path to threads" << std::endl << *stage_)
		tt_->create_thread(Worker(std::tr1::bind(&ExecutionStage::exec_path, stage_, path), *this, id++));
	}

	void plugboard::Boss::run( )
	{
		continue_all();
		sync_post_process();
	}

	void Boss::continue_all()
	{
		assert(tt_ != NULL);

		sync_pre_process();
	}


	void Boss::sync_pre_process() const
	{
		before_process->wait();
	}


	void Boss::sync_post_process() const
	{
		after_process->wait();
	}


	void Boss::shutdown_now()
	{
		shutdown_ = true;
		sync_pre_process();

		PB_DEBUG_MESSAGE_LOCKED("waiting for all threads to terminate.")

		tt_->join_all();

		delete before_process;
		delete after_process;
	}


	bool Boss::must_shutdown() const
	{
		return shutdown_;
	}


	Worker::Worker(const std::tr1::function< void() > f, const Boss& boss, uint32_t id) : f_(f), boss_(boss), id_(id)
	{

	}


	void Worker::operator()() const
	{
		PB_DEBUG_MESSAGE_LOCKED("Worker " << id_ << " started!")

		while(1)
		{
			boss_.sync_pre_process();

			if(boss_.must_shutdown())
			{
				PB_DEBUG_MESSAGE_LOCKED("Worker " << id_ << " exiting!")
				return;
			}

			f_();

			boss_.sync_post_process();
		}
	}
}
