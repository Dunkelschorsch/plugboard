#include "block.hpp"
#include "types.hpp"
#include <iostream>

#define HAS_INPUTS
#undef  HAS_OUTPUTS

static const std::string BLOCK_NAME = "1in";

/* please replace all occurences of '1in' with BLOCK_NAME */

class Block_1in : public Block
{
public:

	Block_1in();

	~Block_1in();

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
	InPort *sig_in_;

	int_vec_t framesize_;
	real_vec_t Ts_;
};


#ifdef HAS_INPUTS
bool Block_1in::setup_input_ports()
{
	/* calls to "add_port(InPort &) go here */
	sig_in_ = add_port(new InPort("in1", empty, Ts_[0], framesize_[0]));

	return true;
}
#endif


#ifdef HAS_OUTPUTS
bool Block_1in::setup_output_ports()
{
	/* calls to "add_port(OutPort &) go here */
	return true;
}
#endif


void Block_1in::configure_parameters()
{
	/* calls to "add_parameter()" go here */
	add_parameter(&Ts_, real, "Sample Time");
	add_parameter(&framesize_, integer, "Frame Size");
}


void Block_1in::process()
{
#ifndef NDEBUG
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
	const integer_t *v_in;
	v_in = get_data_ptr< integer_t >(sig_in_);

#ifndef NDEBUG
	for(uint32_t i=0; i<sig_in_->get_frame_size(); i++)
	{
		std::cout << v_in[i] << " ";
	}
	std::cout << std::endl;
#endif
}


Block_1in::Block_1in()
{
	name_ = BLOCK_NAME;
	set_description("This is a block for testing purposes. It has 1 input.");
	configure_parameters();
#ifndef NDEBUG
	std::cout << "Contructed Block_" << BLOCK_NAME << "!" << std::endl;
#endif
}


Block_1in::~Block_1in()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
}

DEFINE_ACCESS_FUNCTIONS(1in)
