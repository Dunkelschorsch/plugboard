#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include <iostream>



static const std::string BLOCK_NAME = "2in1out";

class Block_2in1out : public Block, public Source, public Sink
{
public:

	Block_2in1out();
	void process();

private:
	void setup_input_ports() ;
	void setup_output_ports() ;

	OutPort *sig_out1_;
	InPort *sig_in1_, *sig_in2_;
};



void Block_2in1out::setup_input_ports()
{
	sig_in1_ = add_port(new InPort("in1", empty, 0, 0));
	sig_in2_ = add_port(new InPort("in2", empty, 0, 0));
}



void Block_2in1out::setup_output_ports()
{
	sig_out1_ = add_port(new OutPort("out1", sig_in1_->get_type(), sig_in1_->get_Ts(), sig_in1_->get_frame_size()));
}




void Block_2in1out::process()
{
#ifndef NDEBUG
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
	int32_t *v_out;
	const int32_t *v_in1, *v_in2;
	
	v_in1 = get_data< int32_t >(sig_in1_);
	v_in2 = get_data< int32_t >(sig_in2_);
	v_out = get_data< int32_t >(sig_out1_);

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


ACCESS_FUNCS(2in1out)
