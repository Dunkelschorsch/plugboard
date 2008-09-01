#ifndef THREAD_MGM_HPP
#define THREAD_MGM_HPP

#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>
#include "exec_stage.hpp"

namespace plugboard
{
	class Boss
	{
	public:
		Boss();
		Boss(boost::thread_group&, ExecutionStage*);
	
		void new_thread_from_path(const ExecutionStage::path_t&);
	
		void continue_all();
		void sync_pre_process() const;
		void sync_post_process() const;
		bool must_shutdown() const;
		void shutdown_now();
	
	private:
		boost::thread_group* tt_;
		ExecutionStage* stage_;
		boost::barrier *before_process, *after_process;
		bool shutdown_;
		static uint32_t id;
	};
	

	class Worker
	{
	public:
		Worker(const boost::function< void() >, const Boss&, uint32_t);
		void operator()() const;
	
	private:
		const boost::function< void() > f_;
		const Boss& boss_;
		uint32_t id_;
	};
}

#endif
