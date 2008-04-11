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

#ifndef EXEC_MATRIX_HPP
#define EXEC_MATRIX_HPP

#include "exec_stage.hpp"
#include "pimpl/pimpl.h"

#include <iosfwd>
#include <map>

namespace plugboard
{
	class ExecutionMatrix : public pimpl< ExecutionMatrix >::pointer_semantics
	{
	public:
		ExecutionMatrix();
		~ExecutionMatrix();

		void store_block(Block * b, const std::string& name);
		bool block_exists(const std::string& name) const;
		void add_block(const std::string& name);
		void place_block(const std::string& name, const std::string& insert_after);
		bool block_is_placed(const std::string& name) const;

		Block* operator[](const std::string& name) const;

		const ExecutionStage::store_t& get_stages() const;
		const std::vector< Block * > find_start_blocks() const;

		void combine_stages();
		void parallelize();
		void init();
		void exec();
		void finalize();

		void print(std::ostream&) const;
	};
} // namespace plugboard


std::ostream& operator<<(std::ostream&, const plugboard::ExecutionMatrix&);

#endif // EXEC_MATRIX_HPP
