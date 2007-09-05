#include "block.hpp"
#include "types.hpp"
#include "port.hpp"
#include <iostream>

#define HAS_INPUTS
#define HAS_OUTPUTS

static const std::string BLOCK_NAME = "2in2out";

class Block_2in2out : public Block
{
public:

	Block_2in2out();

	~Block_2in2out();

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
	OutPort *sig_out1_, *sig_out2_;
	InPort *sig_in1_, *sig_in2_;
};


#ifdef HAS_INPUTS
bool Block_2in2out::setup_input_ports()
{
/* calls to "add_port(InPort &) go here */
	sig_in1_ = add_port(InPort("in1", empty, 0, 0));
	sig_in2_ = add_port(InPort("in2", empty, 0, 0));

	return true;
}
#endif


#ifdef HAS_OUTPUTS
bool Block_2in2out::setup_output_ports()
{
/* calls to "add_port(OutPort &) go here */
	sig_out1_ = add_port(OutPort("out1", integer, 1.0, 2));
	sig_out2_ = add_port(OutPort("out2", integer, 1.0, 2));

	return true;
}
#endif


void Block_2in2out::configure_parameters()
{
/* calls to "add_parameter()" go here */
}


void Block_2in2out::wakeup()
{
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;
}


Block_2in2out::Block_2in2out()
{
	name_ = BLOCK_NAME;
	set_description("This is a block for testing purposes. It has 2 inputs and 2 outputs.");
	configure_parameters();
}


Block_2in2out::~Block_2in2out()
{

}

DEFINE_ACCESS_FUNCTIONS(2in2out)
