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

#ifndef BLOCK_ADD_GET_PORTS_HPP
#define BLOCK_ADD_GET_PORTS_HPP

#include "block/block.hpp"

namespace plugboard
{
	template< class PortT >
	PortT* Block::add_port(PortT * const p)
	{
		typename PortT::store_t *port_list = get_port_list< PortT >();
		typename PortT::store_t::iterator it =
			std::find_if
			(
				port_list->begin(),
				port_list->end(),
				boost::bind(&PortT::get_name, _1) == p->get_name()
			);

		if (it != port_list->end())
		{
			return PortTraits< PortT >::name_exists_action(it, p, this);
		}

		p->set_owner_block_name(get_name_sys());
		port_list->push_back(*p);
		PortTraits< PortT >::increment_no_of_ports(this);
		delete p;

		return &(port_list->back());
	}


	template< >
	inline InPort::store_t * Block::get_port_list< InPort >()
	{
		return dynamic_cast< Sink* >(this)->get_port_list();
	}


	template< >
	inline const InPort::store_t * Block::get_port_list< InPort >() const
	{
		return dynamic_cast< const Sink* >(this)->get_port_list();
	}


	template< >
	inline OutPort::store_t * Block::get_port_list< OutPort >()
	{
		return dynamic_cast< Source* >(this)->get_port_list();
	}


	template< >
	inline const OutPort::store_t * Block::get_port_list< OutPort >() const
	{
		return dynamic_cast< const Source* >(this)->get_port_list();
	}
} // namespace plugboard

#endif // BLOCK_ADD_GET_PORTS_HPP
