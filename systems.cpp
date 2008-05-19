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

#include "systems.hpp"
#include "system.hpp"
#include "subsystem.hpp"


namespace plugboard
{
	Systems::Systems() : root_sys_name_("__root__")
	{
		sys_map_[root_sys_name_].reset(new System);
	}


	Systems & Systems::instance( )
	{
		Systems & s = singleton::instance();
		return s;
	}


	void Systems::add_subsystem(const std::string& subsys_name, Subsystem * const subsys)
	{
		if(sys_map_.find(subsys_name) != sys_map_.end())
		{
			throw("up");
			//TODO come up with a proper exception
		}
		sys_map_.insert(std::make_pair(subsys_name, subsys));
	}


	Subsystem * Systems::get_subsystem( const std::string & subsys_name ) const
	{
		if(sys_map_.find(subsys_name) == sys_map_.end())
		{
			throw("up");
			//TODO come up with a proper exception
		}
		return dynamic_cast< Subsystem* >(sys_map_.find(subsys_name)->second.get());
	}


	Systems::operator System*()
	{
		return sys_map_[root_sys_name_].get();
	}


	System * Systems::get_root( ) const
	{
		return sys_map_.find(root_sys_name_)->second.get();
	}
} // namespace plugboard
