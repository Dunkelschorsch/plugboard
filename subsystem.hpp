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

#ifndef SUBSYSTEM_HPP
#define SUBSYSTEM_HPP

#include "block/block.hpp"
#include "system.hpp"

namespace plugboard
{
	class SubsystemImpl;

	class Subsystem : public System, public Block
	{
	public:
		Subsystem();
		~Subsystem();

		void configure_parameters();
		void initialize();
		void process();

		void create_output(const std::string & block_source, const std::string & port_source, const std::string & port_out);

		void create_input(const std::string & port_in, const std::string & block_sink, const std::string & port_sink);

		bool setup_output_ports();
		bool setup_input_ports();

	protected:
		PB_DECLARE_PROTECTED(Subsystem);

	private:
		PB_DECLARE_PRIVATE(Subsystem);
	};
} // namespace plugboard

#endif // SUBSYSTEM_HPP
