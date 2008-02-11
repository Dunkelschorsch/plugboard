#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include <iostream>
#include <unistd.h>
#include <algorithm>


static const std::string BLOCK_NAME = "1in1out";

class Block_1in1out : public Block, public Source, public Sink
{
public:

	Block_1in1out();
	~Block_1in1out();

	void process();

private:

	void configure_parameters();

	bool setup_input_ports();
	bool setup_output_ports();

	/* member variable declarations go here */
	OutPort *sig_out1_;
	InPort *sig_in1_;
};



bool Block_1in1out::setup_input_ports()
{
	sig_in1_ = add_port(new InPort("in1", empty, 0, 0));

	return true;
}



bool Block_1in1out::setup_output_ports()
{
	type_t out_type = sig_in1_->get_type();
	real_t out_Ts = sig_in1_->get_Ts();
	uint32_t out_size = sig_in1_->get_frame_size();

	sig_out1_ = add_port( new OutPort("out1", out_type, out_Ts, out_size) );
	return true;
}



void Block_1in1out::configure_parameters()
{
/* calls to "add_parameter()" go here */
}


void Block_1in1out::process()
{
#ifndef NDEBUG
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
	int32_t *v_out;
	const int32_t *v_in;
	
	v_in = get_data< int32_t >(sig_in1_);
	v_out = get_data< int32_t >(sig_out1_);

	std::transform
	(
		v_in,
		v_in + sig_in1_->get_frame_size(),
		v_out,
		std::bind1st
		(
			std::plus< int32_t >(),
			1
		)
	);
	
	sig_out1_->send();
}


Block_1in1out::Block_1in1out()
{
	set_name(BLOCK_NAME);
	set_description("This is a block for testing purposes. It has 1 output.");
	configure_parameters();
}


Block_1in1out::~Block_1in1out()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
}

ACCESS_FUNCS(1in1out)
