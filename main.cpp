#include "block_loader.hpp"
#include "block.hpp"
#include "system.hpp"
#include <iostream>



int main(int argc, char **argv)
{
	BlockLoader bl;
	System s;

	bl.load_dir("blocks");

#define COMPLICATED 1
#define SIMPLE 0

	s.add_block(bl.new_block("1in"), "con");
	s.add_block(bl.new_block("1out"), "gen");

	s.add_block(bl.new_block("1in1out"), "br1");
#if COMPLICATED
	s.add_block(bl.new_block("1in1out"), "br2");
	s.add_block(bl.new_block("2in2out"), "cb1");

	s.add_block(bl.new_block("1in2out"), "fo1");
	s.add_block(bl.new_block("2in1out"), "fo2");
#endif

#if SIMPLE
	s.connect_ports("gen", "out1", "br1", "in1");
	
	s.connect_ports("br1", "out1", "con", "in1");
#endif

#if COMPLICATED
	s.connect_ports("gen", "out1", "fo1", "in1"); // 1

	s.connect_ports("fo1", "out1", "cb1", "in1"); // 3
	s.connect_ports("fo1", "out2", "cb1", "in2"); // 3

	s.connect_ports("cb1", "out1", "br1", "in1"); // 2
	s.connect_ports("cb1", "out2", "br2", "in1"); // 4

	s.connect_ports("br1", "out1", "fo2", "in1"); // 5
	s.connect_ports("br2", "out1", "fo2", "in2"); // 6

	s.connect_ports("fo2", "out1", "con", "in1"); // 6
#endif
	s.wakeup_sys(2);

	return EXIT_SUCCESS;
}
