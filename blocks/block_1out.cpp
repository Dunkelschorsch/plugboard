#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"
#include <itpp/itcomm.h>
#include <itpp/itstat.h>


#include <iostream>

#define DLLEXPORT __attribute__ ((visibility("default")))
#define DLLLOCAL  __attribute__ ((visibility("hidden")))


static const std::string BLOCK_NAME = "1out";


class DLLLOCAL Block_1out : public Block, public Source
{
public:

	Block_1out() DLLLOCAL;
	~Block_1out() DLLLOCAL;

	void process() DLLLOCAL;

private:
	void configure_parameters() DLLEXPORT;

	bool setup_output_ports() DLLLOCAL;

	/* member variable declarations go here */
	OutPort *sig_out_;
	int32_vec_t framesize_;
	int32_vec_t constant_;
	real_vec_t Ts_;
};



void Block_1out::configure_parameters()
{
	/* calls to "add_parameter()" go here */
	add_parameter
	(
		(new Parameter(&Ts_, real, "Sample Time"))
		->add_constraint(new GreaterThanConstraint< real_t >(0.0))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&framesize_, int32, "Frame Size"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&constant_, int32, "Output Constant"))
		->add_constraint(new SizeConstraint(1))
	);
}



bool Block_1out::setup_output_ports()
{
	/* calls to "add_port(OutPort &) go here */
	sig_out_ = add_port(new OutPort("out1", int32, Ts_[0], framesize_[0]));

	return true;
}



void Block_1out::process()
{
	itpp::QPSK qpsk;

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
