#include <iostream>
#include "block_loader.hpp"
// #include "block.hpp"
// #include "system.hpp"
// #include "subsystem.hpp"
// #include "variable.hpp"
// #include "exception.hpp"
// #include "symtab.hpp"
#include "input/file.hpp"
// #include "systems.hpp"


int main(int argc, char **argv)
{
	BlockLoader& bl = BlockLoader::instance();
	bl.load_dir("blocks");

// 	Systems& ss = Systems::instance();
// 	ss.add_subsystem("sub1", new Subsystem);
	
// 	Subsystem * subs = Systems::instance().get_subsystem("sub1");
// 	System* root = Systems::instance();


 	HumpFile ff;
 	ff.execute_command(argv[1]);

// 	subs->add_block(BlockLoader::instance().new_block("1in"), "con");


	return EXIT_SUCCESS;
}
