#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include <iostream>


static const std::string BLOCK_NAME = "1in2out";

class Block_1in2out : public Block, public Source, public Sink
{
public:

	Block_1in2out();

	~Block_1in2out();

	void process();

private:

	void configure_parameters() __attribute__ ((visibility("hidden")));

	bool setup_input_ports() __attribute__ ((visibility("hidden")));
	bool setup_output_ports() __attribute__ ((visibility("hidden")));

	/* member variable declarations go here */
	OutPort *sig_out1_, *sig_out2_;
	InPort *sig_in1_;
};


bool Block_1in2out::setup_input_ports()
{
	/* calls to "add_port(InPort &) go here */
	sig_in1_ = add_port(new InPort("in1", empty, 0, 0));

	return true;
}



bool Block_1in2out::setup_output_ports()
{
	/* calls to "add_port(OutPort &) go here */
	sig_out1_ = add_port(new OutPort("out1", sig_in1_->get_type(), sig_in1_->get_Ts(), sig_in1_->get_frame_size()));
	sig_out2_ = add_port(new OutPort("out2", sig_in1_->get_type(), sig_in1_->get_Ts(), sig_in1_->get_frame_size()));

	return true;
}



void Block_1in2out::configure_parameters()
{
	/* calls to "add_parameter()" go here */
}


void Block_1in2out::process()
{
#ifndef NDEBUG
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
	int32_t *v_out1, *v_out2;
	const int32_t *v_in;
	
	v_in = get_data_ptr< int32_t >(sig_in1_);
	v_out1 = get_data_ptr< int32_t >(sig_out1_);
	v_out2 = get_data_ptr< int32_t >(sig_out2_);

	for(uint16_t i=0; i<sig_in1_->get_frame_size(); i++)
	{
		v_out1[i] = v_in[i];
		v_out2[i] = v_in[i];
	}
	
	sig_out1_->send();
	sig_out2_->send();
}


Block_1in2out::Block_1in2out()
{
	set_name(BLOCK_NAME);
	set_description("This is a block for testing purposes. It has 1 inputs and 2 outputs.");
	configure_parameters();
}


Block_1in2out::~Block_1in2out()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
}

ACCESS_FUNCS(1in2out)
