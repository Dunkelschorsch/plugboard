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

#include "block/block_ptr.hpp"
#include <string>
#include "macros.hpp"
#include <boost/pool/detail/singleton.hpp>

namespace plugboard
{
	class Variable;
	struct SystemImpl;

	class System
	{
		typedef boost::details::pool::singleton_default< System > singleton;
		friend class boost::details::pool::singleton_default< System >;
	public:
		virtual ~System();
		
		void add_block(block_ptr const, const std::string&);

		void add_variable(const std::string&, const Variable&);

		const Variable& get_variable(const std::string&) const;

		void connect_ports(const std::string&, const std::string&,
			const std::string&, const std::string&);

		void initialize();

		void wakeup_sys(uint32_t = 1);

		void finalize();

		static System& instance();

	protected:
		PB_DECLARE_PROTECTED(System)

	private:
	  	System();
		PB_DECLARE_PRIVATE(System)
	};
} // namespace plugboard

#endif // SYSTEM_HPP
