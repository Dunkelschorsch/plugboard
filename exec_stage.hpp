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

	std::ostream& operator<<(std::ostream&, const plugboard::ExecutionStage&);
} // namespace plugboard

#endif // EXEC_STAGE_HPP
