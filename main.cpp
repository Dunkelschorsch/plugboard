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

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/thread/mutex.hpp>

#include "block_loader.hpp"
#include "input/file.hpp"
#include "environment.hpp"

// incredibly global variable
boost::mutex pb_io_mutex;

namespace po = boost::program_options;
namespace pb = plugboard;

int main(int argc, char **argv)
{
	bool t_threading = false;
	bool t_show_blocks = false;

	po::options_description desc("Allowed options");

	desc.add_options()
		("help,h", "display this help message")
		("enable-threading", po::bool_switch(&t_threading), "enable multi-threaded execution")
#ifdef NDEBUG
		("verbose-module-load", po::bool_switch(&t_show_blocks), "display which block plugins are being loaded")
#else
		("verbose-module-load", po::bool_switch(&t_show_blocks),
		"display which block plugins are being loaded. CAUTION: This is a debug build. Plugin loading progess is always shown.")
#endif
	;

        po::options_description hidden("hidden options");
	hidden.add_options()
		("input-file", po::value< std::vector < std::string > >(), "input file")
	;

	po::options_description all_options;
	all_options.add(desc).add(hidden);

	po::positional_options_description p;
	p.add("input-file", 1);
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(all_options).positional(p).run(), vm);
	po::notify(vm);

	try
	{
		if (vm.count("help"))
		{
			std::cout << desc << std::endl;
			return 1;
		}

		if (vm.count("input-file") != 1)
		{
			throw std::logic_error("Please specify an input file.");
		} else
		{
			std::string input_file =
				vm["input-file"].as< std::vector < std::string > >()[0];

			pb::Environment::instance().set("threading", t_threading);
			pb::Environment::instance().set("verbose_plugin_load",
#ifdef NDEBUG
										t_show_blocks
#else
										true
#endif
										);
			pb::BlockLoader::instance().load_dir("blocks", true);

			pb::FileInput ff;
			ff.execute_command(input_file);

			return EXIT_SUCCESS;
		}
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 1;
}
