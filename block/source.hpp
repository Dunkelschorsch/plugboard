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

#ifndef BLOCK_SOURCE_HPP
#define BLOCK_SOURCE_HPP

#include <vector>
#include <set>
#include "port/port_out.hpp"
#include "port/port_in.hpp"
#include "visibility.hpp"

namespace plugboard
{
	template< class PortT > class PortTraits;


	class DSOEXPORT Source
	{
	friend class PortTraits< OutPort >;

	public:
		Source();

		inline void call_setup_output_ports() { return setup_output_ports(); }
		uint16_t get_num_output_ports() const;

		OutPort::store_t * get_port_list();
		OutPort::store_t * get_port_list() const;
		const std::set< std::string >& get_connections() const;
		void add_connection(const std::string& name);

		std::vector< std::pair< OutPort::store_t::iterator, InPort::store_t::iterator > > connect_calls;

	protected:
		virtual ~Source();

	private:
		virtual void setup_output_ports() = 0;

		uint16_t num_output_ports_;
		OutPort::store_t ports_out_;
		std::set< std::string > connected_blocks_;
	};
} // namespace plugboard

#endif // BLOCK_SOURCE_HPP
