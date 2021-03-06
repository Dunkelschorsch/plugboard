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

#ifndef PORT_IN_HPP
#define PORT_IN_HPP

#include "port/port_base.hpp"
#include "visibility.hpp"

namespace plugboard
{
	class Message;

	class DSOEXPORT InPort : public BasePort< InPort >
	{
	public:
		InPort(const std::string& name, const type_t type=empty, const real_t Ts=0, const uint32_t frame_size=0);
		const Message receive(uint32_t buff_num);
	};
} // namespace plugboard

#endif // PORT_IN_HPP
