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

#include "source.hpp"

namespace plugboard
	{
	Source::Source() :
		connect_calls(),
		num_output_ports_(0),
		ports_out_(),
		connected_blocks_()
	{ }


	Source::~Source() { }


	uint16_t Source::get_num_output_ports() const
	{
		return num_output_ports_;
	}


	OutPort::store_t * Source::get_port_list( )
	{
		return &ports_out_;
	}


	const std::set< std::string >& Source::get_connections() const
	{
		return connected_blocks_;
	}


	void Source::add_connection(const std::string & name)
	{
		connected_blocks_.insert(name);
	}
} // namespace plugboard
