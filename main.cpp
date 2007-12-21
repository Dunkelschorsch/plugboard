#include <iostream>
#include "block_loader.hpp"
// #include "block.hpp"
// #include "system.hpp"
#include "subsystem.hpp"
// #include "variable.hpp"
// #include "exception.hpp"
// #include "symtab.hpp"
#include "input/console.hpp"
#include "systems.hpp"


int main(int argc, char **argv)
{
	BlockLoader* bbb = &BlockLoader::instance();
	BlockLoader& bl = BlockLoader::instance();
	bbb->load_dir("blocks");

	Systems& ss = Systems::instance();
	ss.add_subsystem("sub1", new Subsystem);
	
	Subsystem * subs = Systems::instance().get_subsystem("sub1");
	System* root = ss;

	subs->add_block(BlockLoader::instance().new_block("1in"), "con");

//	HumpShell shell(root, bl);
//	shell.add_available_blocks(bl);

//	while(shell.execute_command());

#if 1
//	st.add_var("Ts", 1/3.84e6);
//	st.add_var("framesize", 1000);
//	b->set_parameter(st.get_var("Ts"));
//	b->set_parameter(st.get_var("framesize"));
//	root->add_block(b, "hump");
//	return 0;
#define COMPLICATED 1
#define SIMPLE 0
#define LOOP 0


	root->add_block(bl.new_block("1in"), "con");
	root->add_block(bl.new_block("1out"), "gen");
	root->add_block(bl.new_block("1in1out"), "br1");
#if COMPLICATED
	root->add_block(bl.new_block("1in1out"), "br2");
	root->add_block(bl.new_block("1in1out"), "br3");
	root->add_block(bl.new_block("1in1out"), "br4");
	root->add_block(bl.new_block("2in2out"), "cb1");
	root->add_block(bl.new_block("1in2out"), "fo1");
	root->add_block(bl.new_block("2in1out"), "fo2");
#endif

#if SIMPLE
	root->connect_ports("gen", "out1", "br1", "in1");
	root->connect_ports("br1", "out1", "con", "in1");
#endif

#if COMPLICATED
	root->connect_ports("gen", "out1", "fo1", "in1"); // 1
	root->connect_ports("fo2", "out1", "con", "in1"); // 8
	root->connect_ports("cb1", "out2", "br2", "in1"); // 5
	root->connect_ports("fo1", "out1", "cb1", "in1"); // 2
	root->connect_ports("cb1", "out1", "br1", "in1"); // 4
	root->connect_ports("br2", "out1", "br3", "in1"); // 7
	root->connect_ports("br3", "out1", "br4", "in1"); // 7
	root->connect_ports("br4", "out1", "fo2", "in2"); // 7
	root->connect_ports("br1", "out1", "fo2", "in1"); // 6
	root->connect_ports("fo1", "out2", "cb1", "in2"); // 3
#endif

#if LOOP
	root->add_block(bl.new_block("1in1out"), "br2");
	root->add_block(bl.new_block("1in1out"), "br3");
	root->add_block(bl.new_block("1in2out"), "fo1");
	root->add_block(bl.new_block("2in1out"), "fo2");

	root->connect_ports("gen", "out1", "fo2", "in1");
	root->connect_ports("br3", "out1", "fo2", "in2");
	root->connect_ports("fo2", "out1", "br1", "in1");

	root->connect_ports("br1", "out1", "br2", "in1");
	root->connect_ports("br2", "out1", "fo1", "in1");

	root->connect_ports("fo1", "out1", "con", "in1");
	root->connect_ports("fo1", "out2", "br3", "in1");
#endif

	root->initialize();
	root->wakeup_sys(1);
#endif
	return EXIT_SUCCESS;
}
