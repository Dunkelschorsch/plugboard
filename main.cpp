#include <iostream>
#include <boost/program_options.hpp>

#include "block_loader.hpp"
#include "input/file.hpp"



namespace po = boost::program_options;


int main(int argc, char **argv)
{
	bool t_threading;

	try
	{
		po::options_description desc("Allowed options");

		desc.add_options()
			("help,h", "display this help message")
			("threading", po::bool_switch(&t_threading), "enable multi-threaded execution")
		;

	        po::options_description hidden("Hidden options");
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
			throw std::logic_error("Please specify one input file.");
		} else
		{
			std::string input_file =
				vm["input-file"].as< std::vector < std::string > >()[0];
			BlockLoader::instance().load_dir("blocks", true);

			std::cout << "threading is: " << t_threading << std::endl;

			HumpFile ff;
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
