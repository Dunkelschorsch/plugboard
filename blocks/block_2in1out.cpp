#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include <iostream>

#define HAS_INPUTS
#define HAS_OUTPUTS

static const std::string BLOCK_NAME = "2in1out";

class Block_2in1out : public Block, public Source, public Sink
{
public:

	Block_2in1out();

	~Block_2in1out();

	void process();

private:

	void configure_parameters() ;

#ifdef HAS_INPUTS
	bool setup_input_ports() ;
#endif

#ifdef HAS_OUTPUTS
	bool setup_output_ports() ;
#endif

/* member variable declarations go here */
	OutPort *sig_out1_;
	InPort *sig_in1_, *sig_in2_;
};


#ifdef HAS_INPUTS
bool Block_2in1out::setup_input_ports()
{
/* calls to "add_port(InPort &) go here */
	sig_in1_ = add_port(new InPort("in1", empty, 0, 0));
	sig_in2_ = add_port(new InPort("in2", empty, 0, 0));

	return true;
}
#endif


#ifdef HAS_OUTPUTS
bool Block_2in1out::setup_output_ports()
{
/* calls to "add_port(OutPort &) go here */
	sig_out1_ = add_port(new OutPort("out1", sig_in1_->get_type(), sig_in1_->get_Ts(), sig_in1_->get_frame_size()));

	return true;
}
#endif


void Block_2in1out::configure_parameters()
{
/* calls to "add_parameter()" go here */
}


void Block_2in1out::process()
{
#ifndef NDEBUG
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
	int32_t *v_out;
	const int32_t *v_in1, *v_in2;
	
	v_in1 = get_data_ptr< int32_t >(sig_in1_);
	v_in2 = get_data_ptr< int32_t >(sig_in2_);
	v_out = get_data_ptr< int32_t >(sig_out1_);

	for(uint16_t i=0; i<sig_in1_->get_frame_size(); i++)
	{
		v_out[i] = v_in1[i]+v_in2[i];
	}
	
	sig_out1_->send();
}


Block_2in1out::Block_2in1out()
{
	set_name(BLOCK_NAME);
	set_description("This is a block for testing purposes. It has 2 inputs and 1 outputs.");
	configure_parameters();
}


Block_2in1out::~Block_2in1out()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
}

ACCESS_FUNCS(2in1out)
