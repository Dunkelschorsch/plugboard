#include "block_loader.hpp"
#include "system.hpp"

#include <iostream>



int main(int argc, char **argv)
{
	BlockLoader bl;
	System s;

	bl.load_dir("blocks");

	s.add_block(bl.new_block("1in1out"), "third");
	s.add_block(bl.new_block("1in1out"), "fourth");

	s.add_block(bl.new_block("1in"), "second");
	s.add_block(bl.new_block("1out"), "first");

	s.connect_ports("fourth", "out1", "second", "in1");
	s.connect_ports("third", "out1", "fourth", "in1");

	s.connect_ports("first", "out1", "third", "in1");
	
	//s.wakeup_block("first", 1);
	//s.wakeup_block("second", 1);


	return EXIT_SUCCESS;
}
