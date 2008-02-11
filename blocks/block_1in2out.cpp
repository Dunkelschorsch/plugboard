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

	void initialize();
	void process();

private:

	void configure_parameters() ;

	bool setup_input_ports() ;
	bool setup_output_ports() ;

	/* member variable declarations go here */
	OutPort *sig_out1_, *sig_out2_;
	InPort *sig_in1_;

	const itpp::ivec * in_;
	itpp::ivec *out1_, *out2_;
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



void Block_1in2out::initialize( )
{
	in_ = get_signal< int32_t >(sig_in1_);
	out1_ = get_signal< int32_t >(sig_out1_);
	out2_ = get_signal< int32_t >(sig_out2_);
}



void Block_1in2out::configure_parameters()
{
	/* calls to "add_parameter()" go here */
	
}


void Block_1in2out::process()
{
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
	std::cout << " in:   " << *in_ << std::endl;
#endif

	*out1_ = *out2_ = *in_;

	assert(*out1_ == *in_);
	assert(*out2_ == *in_);
#ifndef NDEBUG
	std::cout << " out1: " << *out1_ << std::endl;
	std::cout << " out2: " << *out2_ << std::endl;
#endif

// 	sig_out1_->send();
// 	sig_out2_->send();
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
