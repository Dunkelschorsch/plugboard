#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <iostream>

#undef  HAS_INPUTS
#define HAS_OUTPUTS

static const std::string BLOCK_NAME = "1out";

class Block_1out : public Block, public Source
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
	int32_vec_t framesize_;
	int32_vec_t constant_;
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
	sig_out_ = add_port(new OutPort("out1", int32, Ts_[0], framesize_[0]));

	return true;
}
#endif


void Block_1out::configure_parameters()
{
	/* calls to "add_parameter()" go here */
	add_parameter
	(
		(new Parameter(&Ts_, real, "Sample Time"))
		->add_constraint(new GreaterThanConstraint< real_t >(0.0))
	);

	add_parameter
	(
		(new Parameter(&framesize_, int32, "Frame Size"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
	);

	add_parameter
	(
		new Parameter(&constant_, int32, "Output Constant")
	);
}


void Block_1out::process()
{
#ifndef NDEBUG
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;
#endif

	int32_t *v;
	
	v = get_data_ptr< int32_t >(sig_out_);

	std::fill(v, v+sig_out_->get_frame_size(), constant_[0]);

	sig_out_->send();
}


Block_1out::Block_1out()
{
	set_name(BLOCK_NAME);
	set_description("This is a block for testing purposes. It has 1 output.");
	configure_parameters();
}


Block_1out::~Block_1out()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
}

ACCESS_FUNCS(1out)
