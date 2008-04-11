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

#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include <boost/pool/detail/singleton.hpp>
#include <map>
#include <string>
#include "system.hpp"

namespace plugboard
{
	class Subsystem;

	class Systems
	{
	typedef boost::details::pool::singleton_default< Systems > singleton;
	friend class boost::details::pool::singleton_default< Systems >;

		Systems();
		~Systems();

		std::string root_sys_name_;

		std::map< const std::string, System* > sys_map_;

	public:
		static Systems& instance();

		operator System*();

		void add_subsystem(const std::string& subsys_name, Subsystem * const subsys);

		Subsystem * get_subsystem(const std::string& subsys_name) const;

		System * get_root() const;
	};
} // namespace plugboard

#endif // SYSTEMS_HPP
