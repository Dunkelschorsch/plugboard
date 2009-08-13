/*!
 * \file
 * \brief
 * \author Armin Schmidt
 *
 * ----------------------------------------------------------------------------
 *
 * PlugBoard - A versatile communication simulation framework
 * Copyright (C) 2007-2009  Armin Schmidt
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

#ifndef PLUGBOARD_SWIG_PY
#define PLUGBOARD_SWIG_PY

#include "environment.hpp"
#include "exception/input.hpp"

namespace plugboard { class Variable; }


void __load_blocks(const std::string& path);

void __connect_ports(const std::string& source_block_name,
		const std::string& source_port_name,
		const std::string& sink_block_name,
		const std::string & sink_port_name);
		
void __run(unsigned long times);

void __add_block(const std::string& block_type, const std::string& block_name, plugboard::Variable* v, int n);

#endif

