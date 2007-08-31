#include "block_loader.hpp"
#include "system.hpp"
#include "variable.hpp"

#include <iostream>

/*
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/exponential_distribution.hpp>
#include <boost/random/variate_generator.hpp>
*/

int main(int argc, char **argv)
{
	BlockLoader bl;
	System s;

	bl.load_dir("blocks");

	Block* b = bl.new_block("Dummy2");

	b->set_parameter(1/3.84e6);
	b->set_parameter(3400.5);
	b->set_parameter(10);

	try
	{
		s.add_block(b, "hump1");
	}
	catch (block_not_configured_error &e)
	{
		std::cerr << "block not configured: " << e.what() << std::endl;
	}
	catch (non_existant_block_error &e)
	{
		std::cerr << "non existant block: " << e.what() << std::endl;
	}

	b = bl.new_block("stdout_sink");
	s.add_block(b, "stdout");

	s.connect_ports("hump1", "humpOut1", "stdout", "in");
	s.wakeup_block("hump1", 1);
	s.wakeup_block("stdout", 1);


	//boost::mt19937 generator(42u), generator2;
	//generator2 = generator;
	//boost::exponential_distribution<> exp_dist(1);
	//boost::variate_generator< boost::mt19937&, boost::exponential_distribution<> > uni(generator, exp_dist);

	return EXIT_SUCCESS;
}
