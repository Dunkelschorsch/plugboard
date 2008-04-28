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

#include "port/port_out.hpp"
#include "port/port_in.hpp"
#include "exception/signal.hpp"
#include <tr1/functional>

#include <cassert>

#ifndef NDEBUG
#include <iostream>
#endif


namespace plugboard
{
	OutPort::OutPort(const std::string& name, const type_t type, const real_t Ts, const uint32_t frame_size) :
		BasePort< OutPort >(name, type, Ts, frame_size),
		send(0)
	{

	}



	void OutPort::connect(InPort & other, uint32_t signal_buffer_id)
	{
		assert(get_Ts() > 0.0);
		assert(get_type() != empty);
		assert(get_frame_size() > 0);

		if (other.get_Ts() > 0 && get_Ts() != other.get_Ts())
		{
			throw SampleTimesMismatchException(other.get_owner_block_name()+"::"+other.get_name());
		}

		if (other.get_type() != empty && get_type() != other.get_type())
		{
			throw SignalTypesMismatchException(other.get_owner_block_name()+"::"+other.get_name());
		}

		if (other.get_frame_size() > 0 && get_frame_size() != other.get_frame_size())
		{
			throw FrameSizesMismatchException(other.get_owner_block_name()+"::"+other.get_name());
		}

		assert(get_owner_block_name() != "");

		other.set_Ts(get_Ts());
		other.set_type(get_type());
		other.set_frame_size(get_frame_size());

	#ifndef NDEBUG
		std::cout << "  " << get_owner_block_name() << ".connect(): port '" << get_name() << "' propagating signal type: " << get_type() << std::endl;
		std::cout << "  " << get_owner_block_name() << ".connect(): port '" << get_name() << "' propagating sample time: " << get_Ts() << std::endl;
		std::cout << "  " << get_owner_block_name() << ".connect(): port '" << get_name() << "' propagating frame size:  " << get_frame_size() << std::endl;
	#endif

		signal_buffer_id_ = signal_buffer_id;

		send = std::tr1::bind(&InPort::receive, std::tr1::ref(other), signal_buffer_id);
	}



	void OutPort::disconnect(const InPort &)
	{
		send = 0;
		buffer_access_ = 0;
	}
} // namespace plugboard
