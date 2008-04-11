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

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>
#include "macros.hpp"

namespace plugboard
{
	class Block;
	class Variable;
	struct SystemImpl;

	class System
	{
	public:
		System();
		virtual ~System();

		void add_block(Block * const b, const std::string& name_sys);

		void add_variable(const std::string& name, const Variable& var);

		const Variable& get_variable(const std::string& name) const;

		void connect_ports(const std::string& block_source, const std::string& port_source,
			const std::string& block_sink, const std::string& port_sink);

		void initialize();

		void wakeup_sys(uint32_t times = 1);

		void finalize();

	protected:
		PB_DECLARE_PROTECTED(System)

	private:
		PB_DECLARE_PRIVATE(System)
	};
} // namespace plugboard

#endif // SYSTEM_HPP
