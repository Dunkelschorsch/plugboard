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

#ifndef SYSTEM_IPP
#define SYSTEM_IPP

#include <tr1/functional>
#include <map>

#include "signal/signal.hpp"
#include "exec_matrix.hpp"
#include "symtab.hpp"

namespace plugboard
{
	using std::tr1::function;
	class OutPort;
	class InPort;

	struct SystemImpl
	{
		SystemImpl();
		~SystemImpl();

		void register_basic_types();

		uint32_t create_signal_buffer(type_t type, uint32_t size);
		void set_buffer_ptrs(OutPort& out, InPort& in, Signal* s);

		void linearize(const std::string& block_start);

		void propagate_signal_attributes();

		void execute_row(uint32_t stage_num, uint32_t row_num);

		Signal::store_t signal_buffers_;
		uint32_t signal_buffer_count_;

		typedef std::map< type_t, function< Signal*(size_t) > > signal_assoc_t;
		typedef std::map< type_t, void* (*)(Signal*) > get_buffer_assoc_t;

		signal_assoc_t signal_factory_;
		get_buffer_assoc_t get_buffer_factory_;

		double simulation_time_;

		Symtab symtab_;

		ExecutionMatrix exec_m_;
	};
} // namespace plugboard

#endif // SYSTEM_IPP
