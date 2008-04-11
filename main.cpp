#include <iostream>
#include <boost/program_options.hpp>

#include "block_loader.hpp"
#include "input/file.hpp"
#include "environment.hpp"


namespace po = boost::program_options;
namespace pb = plugboard;

int main(int argc, char **argv)
{
	bool t_threading = false;

	try
	{
		po::options_description desc("Allowed options");

		desc.add_options()
			("help,h", "display this help message")
			("enable-threading", po::bool_switch(&t_threading), "enable multi-threaded execution")
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
			pb::BlockLoader::instance().load_dir("blocks", true);

			pb::Environment::instance().set("threading", t_threading);

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
