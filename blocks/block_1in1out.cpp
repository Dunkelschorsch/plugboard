#include "block.hpp"
#include "types.hpp"
#include <iostream>
#include <unistd.h>

#define HAS_INPUTS
#define HAS_OUTPUTS

static const std::string BLOCK_NAME = "1in1out";

class Block_1in1out : public Block
{
public:

	Block_1in1out();

	~Block_1in1out();

	void process();

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
	sig_in1_ = add_port(new InPort("in1", empty, 0, 0));

	return true;
}
#endif



#ifdef HAS_OUTPUTS
bool Block_1in1out::setup_output_ports()
{
	sig_out1_ = add_port(new OutPort("out1", sig_in1_->get_type(), sig_in1_->get_Ts(), sig_in1_->get_frame_size()));

	return true;
}
#endif


void Block_1in1out::configure_parameters()
{
/* calls to "add_parameter()" go here */
}


void Block_1in1out::process()
{
#ifndef NDEBUG
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
	integer_t *v_out;
	const integer_t *v_in;
	
	v_in = get_data_ptr< integer_t >(sig_in1_);
	v_out = get_data_ptr< integer_t >(sig_out1_);

	for(uint16_t i=0; i<sig_out1_->get_frame_size(); i++)
	{
		v_out[i] = 1+v_in[i];
	}
	
//  	usleep(100000);
	sig_out1_->send();
}


Block_1in1out::Block_1in1out()
{
	name_ = BLOCK_NAME;
	set_description("This is a block for testing purposes. It has 1 output.");
	configure_parameters();
}


Block_1in1out::~Block_1in1out()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
}

DEFINE_ACCESS_FUNCTIONS(1in1out)
