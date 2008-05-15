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

#ifndef BLOCK_BUFFER_ACCESS
#define BLOCK_BUFFER_ACCESS

#include "types/typeinfo.hpp"
#include "port/port.hpp"


namespace plugboard
{
	// direct access to the underlying c-array. use only if you must
	template< class SignalT >
	inline const SignalT* get_data(const InPort *p)
	{
		return static_cast< typename typeinfo< SignalT >::vector_type* >(p->get_signal_ptr())->_data();
	}

	template< class SignalT >
	inline SignalT* get_data(const OutPort *p)
	{
		return static_cast< typename typeinfo< SignalT >::vector_type* >(p->get_signal_ptr())->_data();
	}

	template< class SignalT >
	inline const typename typeinfo< SignalT >::vector_type* get_signal(const InPort *p)
	{
		return static_cast< const typename typeinfo< SignalT >::vector_type* >(p->get_signal_ptr());
	}

	template< class SignalT >
	inline typename typeinfo< SignalT >::vector_type* get_signal(const OutPort *p)
	{
		return static_cast< typename typeinfo< SignalT >::vector_type* >(p->get_signal_ptr());
	}
} // namespace plugboard

#endif // BLOCK_BUFFER_ACCESS
