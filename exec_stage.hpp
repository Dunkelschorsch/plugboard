#ifndef EXEC_STAGE_HPP
#define EXEC_STAGE_HPP

#include <list>
#include <iosfwd>

#include "pimpl/pimpl.h"
#include "block/block.hpp"

namespace boost { class thread; }


namespace plugboard
{
	class ExecutionStage : public pimpl< ExecutionStage >::pointer_semantics
	{
	public:
		typedef std::list< ExecutionStage > store_t;
		typedef Block::store_t path_t;
		typedef std::list< path_t > stage_t;

		ExecutionStage();

		explicit ExecutionStage(Block * const b, bool threading_enabled=false);

		void add_block(Block * const b);
		void add_path(const path_t& p);

		bool block_is_placed(const std::string& name) const;

		const stage_t& get_paths() const;
		stage_t& get_paths();

		Block* operator[](const std::string& name) const;

		void exec();
		void exec_path(const path_t& p);

		void print(std::ostream&) const;
	};
} // namespace plugboard


std::ostream& operator<<(std::ostream&, const plugboard::ExecutionStage&);

#endif // EXEC_STAGE_HPP
