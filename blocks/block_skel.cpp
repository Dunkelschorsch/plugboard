#include "block.hpp"
#include "types.hpp"
#include "port.hpp"

#include <iostream>

#define HAS_INPUTS
#define HAS_OUTPUTS

static const std::string BLOCK_NAME = "changeme!";

/* please replace all occurences of '$NAME' with BLOCK_NAME or something similar */

class Block_$NAME : public Block
{
public:

	Block_$NAME();

	~Block_$NAME();

	void wakeup();

private:

	void configure_parameters() __attribute__ ((visibility("hidden")));

#ifdef HAS_INPUTS
	void setup_input_ports() __attribute__ ((visibility("hidden")));
#endif

#ifdef HAS_OUTPUTS
	void setup_output_ports() __attribute__ ((visibility("hidden")));
#endif

/* member variable declarations go here */

};


#ifdef HAS_INPUTS
void Block_$NAME::setup_input_ports()
{
/* calls to "add_port(InPort &) go here */
}
#endif


#ifdef HAS_OUTPUTS
void Block_$NAME::setup_output_ports()
{
/* calls to "add_port(OutPort &) go here */
}
#endif


void Block_$NAME::configure_parameters()
{
/* calls to "add_parameter()" go here */
}


void Block_$NAME::wakeup()
{

}


Block_$NAME::Block_$NAME()
{
	name_ = BLOCK_NAME;
	configure_parameters();
}


Block_$NAME::~Block_$NAME()
{

}


extern "C"
Block* create()
{
	return new Block_$NAME;
}


extern "C"
const string_t name()
{
	return BLOCK_NAME;
}
