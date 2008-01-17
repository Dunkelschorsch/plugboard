#include "block.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include <iostream>

#undef  HAS_INPUTS
#define HAS_OUTPUTS

static const std::string BLOCK_NAME = "1out";

class Block_1out : public Block
{
public:

	Block_1out();

	~Block_1out();

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
	OutPort *sig_out_;
	int_vec_t framesize_;
	int_vec_t constant_;
	real_vec_t Ts_;
};


#ifdef HAS_INPUTS
bool Block_1out::setup_input_ports()
{
	/* calls to "add_port(InPort &) go here */
	return true;
}
#endif


#ifdef HAS_OUTPUTS
bool Block_1out::setup_output_ports()
{
	/* calls to "add_port(OutPort &) go here */
	sig_out_ = add_port(new OutPort("out1", integer, Ts_[0], framesize_[0]));

	return true;
}
#endif


void Block_1out::configure_parameters()
{
	/* calls to "add_parameter()" go here */
	add_parameter(&Ts_, real, "Sample Time");
	add_parameter(&framesize_, integer, "Frame Size");
	add_parameter(&constant_, integer, "Output Constant");
}


void Block_1out::process()
{
#ifndef NDEBUG
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;
#endif

	integer_t *v;
	
	v = get_data_ptr< integer_t >(sig_out_);

	std::fill(v, v+sig_out_->get_frame_size(), constant_[0]);

	sig_out_->send();
}


Block_1out::Block_1out()
{
	name_ = BLOCK_NAME;
	set_description("This is a block for testing purposes. It has 1 output.");
	configure_parameters();
}


Block_1out::~Block_1out()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
}

DEFINE_ACCESS_FUNCTIONS(1out)
