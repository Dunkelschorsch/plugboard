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

#include "port/port_in.hpp"
#include "message.hpp"

#ifndef NDEBUG
#include <iostream>
#endif


namespace plugboard
{
	InPort::InPort(const std::string& name, const type_t type, const real_t Ts, const uint32_t frame_size) :
		BasePort< InPort >(name, type, Ts, frame_size)
	{

	}



	const Message InPort::receive(uint32_t buff_num)
	{
#ifndef NDEBUG
		std::cout << "Hurra!! Geht!!!! Portname: '" << get_name() << "' buff_num: " << buff_num << 
			" Ts: " << get_Ts() << " framesize: " << get_frame_size() << std::endl;
#endif
		return Message();
	}
} // namespace plugboard
