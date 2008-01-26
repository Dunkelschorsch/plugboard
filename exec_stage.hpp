#ifndef EXEC_STAGE_HPP
#define EXEC_STAGE_HPP

#include <list>
#include <iosfwd>

#include "block/block.hpp"

namespace boost { class thread; }



class ExecutionStage
{
friend class ExecutionMatrix;
public:
	typedef std::list< ExecutionStage > store_t;
	typedef Block::store_t path_t;
	typedef std::list< path_t > stage_t;

	ExecutionStage();

	explicit ExecutionStage(Block * const b, bool threading_enabled=false);

	void add_block(Block * const b);
	void add_path(const path_t& p);

	const stage_t& get_paths() const;
	stage_t& get_paths();

	Block* operator[](const std::string& name) const;

	void print(std::ostream&) const;

protected:
	stage_t paths_;
	bool threading_enabled_;
	std::vector< boost::thread* > thread_group_;

	bool block_is_placed(const std::string& name) const;

	void exec();
	void exec_path(const path_t& p);
};



std::ostream& operator<<(std::ostream&, const ExecutionStage&);

#endif // EXEC_STAGE_HPP
