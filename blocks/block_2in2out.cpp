#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include <iostream>

#define HAS_INPUTS
#define HAS_OUTPUTS

static const std::string BLOCK_NAME = "2in2out";

class Block_2in2out : public Block, public Source, public Sink
{
public:

	Block_2in2out();

	~Block_2in2out();

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
	OutPort *sig_out1_, *sig_out2_;
	InPort *sig_in1_, *sig_in2_;
};


#ifdef HAS_INPUTS
bool Block_2in2out::setup_input_ports()
{
/* calls to "add_port(InPort &) go here */
	sig_in1_ = add_port(new InPort("in1"));
	sig_in2_ = add_port(new InPort("in2"));

	return true;
}
#endif


#ifdef HAS_OUTPUTS
bool Block_2in2out::setup_output_ports()
{
/* calls to "add_port(OutPort &) go here */
	sig_out1_ = add_port(new OutPort("out1", sig_in1_->get_type(), sig_in1_->get_Ts(), sig_in1_->get_frame_size()));
	sig_out2_ = add_port(new OutPort("out2", sig_in2_->get_type(), sig_in2_->get_Ts(), sig_in2_->get_frame_size()));

	return true;
}
#endif


void Block_2in2out::configure_parameters()
{
/* calls to "add_parameter()" go here */
}


void Block_2in2out::process()
{
#ifndef NDEBUG
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;
#endif

	int32_t *v_out1, *v_out2;
	const int32_t *v_in1, *v_in2;
	
	v_in1 = get_data< int32_t >(sig_in1_);
	v_in2 = get_data< int32_t >(sig_in2_);

	v_out1 = get_data< int32_t >(sig_out1_);
	v_out2 = get_data< int32_t >(sig_out2_);

	std::transform
	(
		v_in1,
		v_in1 + sig_in1_->get_frame_size(),
		v_out1,
		std::bind1st
		(
			std::plus< int32_t >(),
			1
		)
	);

	std::transform
	(
		v_in2,
		v_in2 + sig_in2_->get_frame_size(),
		v_out2,
		std::bind1st
		(
			std::plus< int32_t >(),
			2
		)
	);
	
	sig_out1_->send();
	sig_out2_->send();
}


Block_2in2out::Block_2in2out()
{
	set_name(BLOCK_NAME);
	set_description("This is a block for testing purposes. It has 2 inputs and 2 outputs.");
	configure_parameters();
}


Block_2in2out::~Block_2in2out()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
}

ACCESS_FUNCS(2in2out)
