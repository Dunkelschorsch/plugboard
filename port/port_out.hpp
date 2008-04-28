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

#ifndef PORT_OUT_HPP
#define PORT_OUT_HPP

#include "port/port_base.hpp"
#include "message.hpp"
#include "visibility.hpp"

namespace plugboard
{
	class InPort;

	class DSOEXPORT OutPort : public BasePort< OutPort >
	{
	public:
		OutPort(const std::string& name, const type_t type, const real_t Ts, const uint32_t frame_size);
		void connect(InPort& other, uint32_t signal_buffer_id);
		void disconnect(const InPort&);
		std::tr1::function< Message() > send;
	};
} // namespace plugboard

#endif // PORT_OUT_HPP
