#include "block_loader.hpp"
#include "block.hpp"
#include "system.hpp"
#include <iostream>



int main(int argc, char **argv)
{
	BlockLoader bl;
	System s;

	bl.load_dir("blocks");

	
	s.add_block(bl.new_block("1in"), "con");

	s.add_block(bl.new_block("1in1out"), "br1");
	s.add_block(bl.new_block("1in1out"), "br2");

	s.add_block(bl.new_block("1out"), "gen");

	
	s.connect_ports("gen", "out1", "br1", "in1");
	s.connect_ports("br1", "out1", "br2", "in1");

	s.connect_ports("br2", "out1", "con", "in1");
	s.wakeup_sys();

	return EXIT_SUCCESS;
}
