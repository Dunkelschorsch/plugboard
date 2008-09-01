#include "thread_mgm.hpp"
#include <boost/bind.hpp>
#ifndef NDEBUG
#include <iostream>
#endif

// this one is declared in main.cpp
extern boost::mutex pb_io_mutex;

namespace plugboard
{
	uint32_t Boss::id = 0;


	Boss::Boss() : tt_(NULL) { }


	Boss::Boss(boost::thread_group& tt, ExecutionStage* stage) : tt_(&tt), stage_(stage), shutdown_(false)
	{
#ifndef NDEBUG
		{
			const boost::mutex::scoped_lock lock(pb_io_mutex);
			std::cout << "creating rendezvous points for " << stage_->get_paths().size() << " threads." << std::endl;
		}
#endif

		before_process = new boost::barrier(1 + stage_->get_paths().size());
		after_process = new boost::barrier(1 + stage_->get_paths().size());

#ifndef NDEBUG
		{
			const boost::mutex::scoped_lock lock(pb_io_mutex);
			std::cout << "creating manager object." << std::endl;
		}
#endif
	}


	void Boss::new_thread_from_path(const ExecutionStage::path_t &path)
	{
		tt_->create_thread(Worker(boost::bind(&ExecutionStage::exec_path, stage_, path), *this, id++));
#ifndef NDEBUG
		{
			const boost::mutex::scoped_lock lock(pb_io_mutex);
			std::cout << "adding path to threads" << std::endl;
		}
		std::cout << *stage_ << std::endl;
#endif
	}


	void Boss::continue_all()
	{
		assert(tt_ != NULL);
#ifndef NDEBUG
		{
			const boost::mutex::scoped_lock lock(pb_io_mutex);
			std::cout << "Manager wakes up all worker threads!" << std::endl;
		}
#endif
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
#ifndef NDEBUG
		{
			const boost::mutex::scoped_lock lock(pb_io_mutex);
			std::cout << "waiting for all threads to terminate." << std::endl;
		}
#endif
		tt_->join_all();

		delete before_process;
		delete after_process;
	}


	bool Boss::must_shutdown() const
	{
		return shutdown_;
	}


	Worker::Worker(const boost::function< void() > f, const Boss& boss, uint32_t id) : f_(f), boss_(boss), id_(id)
	{

	}


	void Worker::operator()() const
	{
#ifndef NDEBUG
		{
			const boost::mutex::scoped_lock lock(pb_io_mutex);
			std::cout << "Worker " << id_ << " started!" << std::endl;
		}
#endif

		while(1)
		{
#ifndef NDEBUG
			{
				const boost::mutex::scoped_lock lock(pb_io_mutex);
				std::cout << "Worker " << id_ << " waiting at pre-process barrier!" << std::endl;
			}
#endif
			boss_.sync_pre_process();

			if(boss_.must_shutdown())
			{
#ifndef NDEBUG
				{
					const boost::mutex::scoped_lock lock(pb_io_mutex);
					std::cout << "Worker " << id_ << " exiting!" << std::endl;
				}
#endif
				return;
			}

#ifndef NDEBUG
			{
				const boost::mutex::scoped_lock lock(pb_io_mutex);
				std::cout << "Worker " << id_ << " working..." << std::endl;
			}
#endif
			f_();
#ifndef NDEBUG
			{
				const boost::mutex::scoped_lock lock(pb_io_mutex);
				std::cout << "Worker " << id_ << " waiting at post-process barrier!" << std::endl;
			}
#endif
			boss_.sync_post_process();
		}
	}
}
