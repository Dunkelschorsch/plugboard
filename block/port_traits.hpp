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

#ifndef PORT_TRAITS_HPP
#define PORT_TRAITS_HPP

#ifndef NDEBUG
#include <iostream>
#endif

#include "exception/port.hpp"
#include "block/source.hpp"
#include "block/sink.hpp"


namespace plugboard
{
	class Block;
	class OutPort;

	template< class PortT > class PortTraits;

	// specialization for input ports
	template< >
	struct PortTraits< InPort >
	{
		template< class IteratorT, typename PointerT >
		static InPort* name_exists_action(IteratorT it, InPort * const p, const PointerT * const self);

		template< typename PointerT >
		static void increment_no_of_ports(PointerT * const self);
	};


	template< class IteratorT, typename PointerT >
	InPort* PortTraits< InPort >::name_exists_action(IteratorT, InPort * const p, const PointerT * const self)
	{
		throw DuplicatePortNameException(self->get_name() + "::" + p->get_name());
		return NULL;
	}


	template< typename PointerT >
	void PortTraits< InPort >::increment_no_of_ports(PointerT * const self)
	{
		// this can go unchecked due to the specialization of PortT to InPort
		dynamic_cast< Sink* >(self)->num_input_ports_++;
	}



	// specialization for output ports
	template< >
	struct PortTraits< OutPort >
	{
		template< class IteratorT, typename PointerT >
		static OutPort* name_exists_action(IteratorT it, OutPort * const p, const PointerT * const self);

		template< typename PointerT >
		static void increment_no_of_ports(PointerT* const self);
	};


	template< class IteratorT, typename PointerT >
	OutPort* PortTraits< OutPort >::name_exists_action
#ifndef NDEBUG
	(IteratorT it, OutPort * const p, const PointerT * const self)
#else
	(IteratorT it, OutPort * const p, const PointerT * const)
#endif
	{
	#ifndef NDEBUG
		std::cout << "    " << self->get_name_sys() <<
			".add_port(OutPort*): setting output port type to: " << p->get_type() << std::endl;
	#endif
		type_t t = p->get_type();
		real_t Ts = p->get_Ts();
		int32_t framesize = p->get_frame_size();

		// propagating a default value is certainly an error
		assert(t != empty);
		assert(Ts > 0.0);
		assert(framesize > 0);

		it->set_type(t);
		it->set_frame_size(framesize);
		it->set_Ts(Ts);

		delete p;
		return &(*it);
	}


	template< typename PointerT >
	void PortTraits< OutPort >::increment_no_of_ports(PointerT* const self)
	{
		// this can go unchecked due to the specialization of PortT to OutPort
		dynamic_cast< Source* >(self)->num_output_ports_++;
	}
} // namespace plugboard

#endif // PORT_TRAITS_HPP
