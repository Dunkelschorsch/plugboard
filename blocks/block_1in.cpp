#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include <iostream>



static const std::string BLOCK_NAME = "1in";

/* please replace all occurences of '1in' with BLOCK_NAME */

class Block_1in : public Block, public Sink
{
public:

	Block_1in();

	~Block_1in();

	void process();

private:

	void configure_parameters();

	bool setup_input_ports();
	bool setup_output_ports();


/* member variable declarations go here */
	InPort *sig_in_;

	int32_vec_t framesize_;
	real_vec_t Ts_;
};



bool Block_1in::setup_input_ports()
{
	/* calls to "add_port(InPort &) go here */
	sig_in_ = add_port(new InPort("in1", empty, Ts_[0], framesize_[0]));

	return true;
}



void Block_1in::configure_parameters()
{
	/* calls to "add_parameter()" go here */
	add_parameter(&Ts_, real, "Sample Time");
	add_parameter(&framesize_, int32, "Frame Size");
}


void Block_1in::process()
{
#ifndef NDEBUG
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
	const int32_t *v_in;
	v_in = get_data_ptr< int32_t >(sig_in_);

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

ACCESS_FUNCS(1in)
