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

#ifndef BLOCK_SINK_HPP
#define BLOCK_SINK_HPP

#include "port/port_in.hpp"
#include "visibility.hpp"

namespace plugboard
{
	template< class PortT > class PortTraits;

	class DSOEXPORT Sink
	{
	friend class PortTraits< InPort >;

	public:
		Sink();

		inline void call_setup_input_ports() { setup_input_ports(); }
		uint16_t get_num_input_ports() const;

		InPort::store_t * get_port_list();
		InPort::store_t * get_port_list() const;

	protected:
		virtual ~Sink();

	private:
		virtual void setup_input_ports() = 0;
		uint16_t num_input_ports_;
		InPort::store_t ports_in_;
	};
} // namespace plugboard

#endif // BLOCK_SINK_HPP
