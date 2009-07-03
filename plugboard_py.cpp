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


#include "environment.hpp"
#include "exception/input.hpp"
#include "block_loader.hpp"
#include "block/block.hpp"
#include "block/block_ptr.hpp"
#include "systems.hpp"
#include "variable/variable.hpp"

#include <boost/python.hpp>
#include <boost/python/numeric.hpp>
#include <boost/thread/mutex.hpp>


#define PB_DEBUG_MESSAGE_COLOUR \033[01;32m
#define PB_DEBUG_MESSAGE_SOURCE configuration

#include "colour_debug.hpp"


namespace py = boost::python;
namespace pb = plugboard;

boost::mutex pb_io_mutex;

void load_blocks(const std::string& path)
{
	pb::BlockLoader::instance().load_dir(path, true);
}


char dtypecode(const py::numeric::array& a)
{
	return py::extract< char >(a.attr("dtype").attr("char"));
}


template< class T >
void fill_variable_from_array(pb::Variable& v, const py::numeric::array& a)
{
	uint64_t numel = py::extract< uint64_t >(a.attr("size"));
	for(uint64_t i=0; i<numel; i++)
	{
		v.push_back(static_cast< T >(py::extract< T >(a[i])));
	}
}


void add_block(const std::string& block_type, const std::string& block_name, py::tuple block_args)
{
	uint32_t num_args = py::len(block_args);

	PB_DEBUG_MESSAGE("adding block")
	PB_DEBUG_MESSAGE("type: " << block_type)
	PB_DEBUG_MESSAGE("name: " << block_name)
	PB_DEBUG_MESSAGE("no. of arguments given: " << num_args)

	pb::block_ptr b(pb::BlockLoader::instance().new_block(block_type));
	b->call_configure_parameters();

	if(b->get_params().size() != num_args)
	{
		throw pb::ParameterCountMismatchException(block_name);
	}

	// fill block with provided parameters
	for(uint32_t param_num=0; param_num<num_args; ++param_num)
	{
		pb::Variable param_curr;

		py::numeric::array a(block_args[param_num]);
		char typecode = dtypecode(a);

		PB_DEBUG_MESSAGE("found parameter type: " << typecode)

		switch(typecode)
		{
		#define BOOST_PP_DEF(z, I, _)						\
			case NUMPY_TYPE(I):						\
				fill_variable_from_array< CPP_TYPE(I) >(param_curr, a);	\
				break;							
			BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)
		#undef BOOST_PP_DEF
			default:
				assert(0);
		}

		assert(param_curr);

		b->set_parameter(param_curr);
	}

	assert(b->is_configured());

	pb::Systems::instance().get_root()->add_block(b, block_name);
}


void add_block_no_args(const std::string& block_type, const std::string& block_name)
{
	add_block(block_type, block_name, py::tuple());
}


void connect_ports(const std::string& source_block_name,
		const std::string& source_port_name,
		const std::string& sink_block_name,
		const std::string & sink_port_name)
{
	PB_DEBUG_MESSAGE("connecting blocks " << source_block_name << ":" << source_port_name << "->" 
		<< sink_block_name << ":" << sink_port_name)

	pb::Systems::instance().get_root()->connect_ports(source_block_name, source_port_name, sink_block_name, sink_port_name);
}


void run(uint64_t times)
{
	PB_DEBUG_MESSAGE("running system " << times << " times")
	PB_DEBUG_MESSAGE("initializing...")

	pb::Systems::instance().get_root()->initialize();

	PB_DEBUG_MESSAGE("starting system...")

	pb::Systems::instance().get_root()->wakeup_sys(times);

	PB_DEBUG_MESSAGE("finalizing system...")

	pb::Systems::instance().get_root()->finalize();
}


void translator(pb::Exception< std::string > const& e)
{
	std::string err_msg(e.what());

	if(not e.get_id().empty())
		err_msg += ": »" + e.get_id() + "«";

	PyErr_SetString(PyExc_RuntimeError, err_msg.c_str());
}


BOOST_PYTHON_MODULE(libplugboard)
{
	py::numeric::array::set_module_and_type("numpy", "ndarray");
	pb::Environment::instance().set("verbose_plugin_load", false);

	py::register_exception_translator< pb::Exception< std::string > >(translator);

	py::def("load_blocks", &load_blocks);
	py::def("add_block", &add_block);
	py::def("add_block", &add_block_no_args);
	py::def("connect", &connect_ports);
	py::def("run", &run);
}

#undef PB_DEBUG_MESSAGE
#undef PB_DEBUG_MESSAGE_LOCKED
