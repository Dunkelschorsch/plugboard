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

#include "plugboard.hpp"
#include "block_loader.hpp"
#include "block/block.hpp"
#include "block/block_ptr.hpp"
#include "variable/variable.hpp"
#include "systems.hpp"

#define PB_DEBUG_MESSAGE_COLOUR \033[01;32m
#define PB_DEBUG_MESSAGE_SOURCE configuration
#include "colour_debug.hpp"

#include <boost/thread/mutex.hpp>
boost::mutex pb_io_mutex;

namespace pb = plugboard;


void __load_blocks(const std::string& path)
{
	pb::BlockLoader::instance().load_dir(path, true);
}


void __connect_ports(const std::string& source_block_name,
		const std::string& source_port_name,
		const std::string& sink_block_name,
		const std::string & sink_port_name)
{
	PB_DEBUG_MESSAGE("connecting blocks " << source_block_name << ":" << source_port_name << "->" 
		<< sink_block_name << ":" << sink_port_name)

	pb::Systems::instance().get_root()->connect_ports(source_block_name, source_port_name, sink_block_name, sink_port_name);
}


void __run(unsigned long times)
{
	PB_DEBUG_MESSAGE("running system " << times << " times")
	PB_DEBUG_MESSAGE("initializing...")

	pb::Systems::instance().get_root()->initialize();

	PB_DEBUG_MESSAGE("starting system...")

	pb::Systems::instance().get_root()->wakeup_sys(times);

	PB_DEBUG_MESSAGE("finalizing system...")

	pb::Systems::instance().get_root()->finalize();
}


void __add_block(const std::string& block_type, const std::string& block_name, plugboard::Variable* block_args, int n)
{
	uint32_t num_args = n;

	PB_DEBUG_MESSAGE("adding block")
	PB_DEBUG_MESSAGE("type: " << block_type)
	PB_DEBUG_MESSAGE("name: " << block_name)
	PB_DEBUG_MESSAGE("no. of arguments given: " << num_args)

	plugboard::block_ptr b(pb::BlockLoader::instance().new_block(block_type));
	b->call_configure_parameters();

	if(b->get_params().size() != num_args)
	{
		throw plugboard::ParameterCountMismatchException(block_name);
	}
	PB_DEBUG_MESSAGE("filling block with parameters")

	// fill block with provided parameters
	for(uint32_t param_num=0; param_num<num_args; ++param_num)
	{
		assert(block_args[param_num]);
		b->set_parameter(block_args[param_num]);
	}

	assert(b->is_configured());

	plugboard::Systems::instance().get_root()->add_block(b, block_name);
}

