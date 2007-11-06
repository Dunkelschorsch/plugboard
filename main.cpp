#include <iostream>
#include "block_loader.hpp"
#include "block.hpp"
#include "system.hpp"
#include "variable.hpp"
#include "exceptions.hpp"
#include "symtab.hpp"
#include "input/console.hpp"


int main(int argc, char **argv)
{
	BlockLoader bl;
	System s;

	bl.load_dir("blocks");
// 	std::string st;

// 	s.add_block(bl.new_block("1out"), "source1");
// 	s.add_block(bl.new_block("1out"), "source2");
// 	s.add_block(bl.new_block("1in"), "sink1");
// 	s.add_block(bl.new_block("1in"), "sink2");
// 	s.add_block(bl.new_block("2in2out"), "ding");
// 	s.connect_ports("source1", "out1", "ding", "in1");
// 	s.connect_ports("source2", "out1", "ding", "in2");
// 	s.connect_ports("ding", "out1", "sink1", "in1");
// 	s.connect_ports("ding", "out2", "sink2", "in1");
// 	s.initialize();
// 	return EXIT_SUCCESS;


	HumpShell shell(s, bl);

	shell.add_available_blocks(bl);

	while(shell.execute_command());

#if 0
// 	try {
// 		b = bl.new_block("Dummy2");
// 	}
// 	catch (InvalidBlockIdException &e)
// 	{
// 		std::cout << e.what() << ": '" << e.get_id() << "'" << std::endl;
// 	}
// 	Symtab st;
// 
// 	st.add_var("Ts", 1/3.84e6);
// 	st.add_var("framesize", 1000);
// 	b->set_parameter(st.get_var("Ts"));
// 	b->set_parameter(st.get_var("framesize"));
// 	s.add_block(b, "hump");
	return 0;
#define COMPLICATED 1
#define SIMPLE 0
#define LOOP 0


	s.add_block(bl.new_block("1in"), "con");
	s.add_block(bl.new_block("1out"), "gen");
	s.add_block(bl.new_block("1in1out"), "br1");
#if COMPLICATED
	s.add_block(bl.new_block("1in1out"), "br2");
	s.add_block(bl.new_block("1in1out"), "br3");
	s.add_block(bl.new_block("1in1out"), "br4");
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
	s.connect_ports("fo2", "out1", "con", "in1"); // 8
	s.connect_ports("cb1", "out2", "br2", "in1"); // 5
	s.connect_ports("fo1", "out1", "cb1", "in1"); // 2
	s.connect_ports("cb1", "out1", "br1", "in1"); // 4
	s.connect_ports("br2", "out1", "br3", "in1"); // 7
	s.connect_ports("br3", "out1", "br4", "in1"); // 7
	s.connect_ports("br4", "out1", "fo2", "in2"); // 7
	s.connect_ports("br1", "out1", "fo2", "in1"); // 6
	s.connect_ports("fo1", "out2", "cb1", "in2"); // 3
#endif

#if LOOP
	s.add_block(bl.new_block("1in1out"), "br2");
	s.add_block(bl.new_block("1in1out"), "br3");
	s.add_block(bl.new_block("1in2out"), "fo1");
	s.add_block(bl.new_block("2in1out"), "fo2");

	s.connect_ports("gen", "out1", "fo2", "in1");
	s.connect_ports("br3", "out1", "fo2", "in2");
	s.connect_ports("fo2", "out1", "br1", "in1");

	s.connect_ports("br1", "out1", "br2", "in1");
	s.connect_ports("br2", "out1", "fo1", "in1");

	s.connect_ports("fo1", "out1", "con", "in1");
	s.connect_ports("fo1", "out2", "br3", "in1");
#endif

	s.initialize();
	s.wakeup_sys(1);
#endif
	return EXIT_SUCCESS;
}
