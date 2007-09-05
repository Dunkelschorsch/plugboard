#include "block.hpp"
#include "types.hpp"
#include "port.hpp"
#include <iostream>

#define HAS_INPUTS
#define HAS_OUTPUTS

static const std::string BLOCK_NAME = "1in1out";

class Block_1in1out : public Block
{
public:

	Block_1in1out();

	~Block_1in1out();

	void wakeup();

private:

	void configure_parameters() __attribute__ ((visibility("hidden")));

#ifdef HAS_INPUTS
	bool setup_input_ports() __attribute__ ((visibility("hidden")));
#endif

#ifdef HAS_OUTPUTS
	bool setup_output_ports() __attribute__ ((visibility("hidden")));
#endif

/* member variable declarations go here */
	OutPort *sig_out1_;
	InPort *sig_in1_;
};


#ifdef HAS_INPUTS
bool Block_1in1out::setup_input_ports()
{
/* calls to "add_port(InPort &) go here */
	sig_in1_ = add_port(InPort("in1", empty, 0, 0));

	return true;
}
#endif


#ifdef HAS_OUTPUTS
bool Block_1in1out::setup_output_ports()
{
/* calls to "add_port(OutPort &) go here */
	sig_out1_ = add_port(OutPort("out1", integer, 1.0, 2));

	return true;
}
#endif


void Block_1in1out::configure_parameters()
{
/* calls to "add_parameter()" go here */
}


void Block_1in1out::wakeup()
{
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;
}


Block_1in1out::Block_1in1out()
{
	name_ = BLOCK_NAME;
	set_description("This is a block for testing purposes. It has 1 output.");
	configure_parameters();
}


Block_1in1out::~Block_1in1out()
{

}

DEFINE_ACCESS_FUNCTIONS(1in1out)
