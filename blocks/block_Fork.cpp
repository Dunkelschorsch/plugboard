#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <iostream>
#include <cstdio>

static const std::string BLOCK_NAME = "Fork";

class Block_Fork : public Block, public Source, public Sink
{
public:

	Block_Fork();
	~Block_Fork();

	void initialize();
	void process();

private:
	void setup_input_ports();
	void setup_output_ports();
	void configure_parameters();

	template< typename T >
	void do_fork();

	OutPort **sig_out_;
	InPort *sig_in1_;

	const void *v_in_;
	void **v_out_;

	int32_vec_t num_outputs_;
	type_t input_type_;
};


void Block_Fork::configure_parameters( )
{
	add_parameter
	(
		(new Parameter(&num_outputs_, int32, "Number of outputs"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new LessThanConstraint< int32_t >(1000))
		->add_constraint(new SizeConstraint(1))
	);
}


void Block_Fork::setup_input_ports()
{
	/* calls to "add_port(InPort &) go here */
	sig_in1_ = add_port(new InPort("in1", empty, 0, 0));
}


void Block_Fork::setup_output_ports()
{
	// only reserve that memory once
	if(num_output_ports_ != num_outputs_[0])
		sig_out_ = new OutPort*[num_outputs_[0]];

	for(int32_t i=0; i<num_outputs_[0]; ++i)
	{
		char str[3];
		std::sprintf(str, "%d", i+1);
		std::string portname = std::string("out")+std::string(str);

		sig_out_[i] = add_port(new OutPort(portname,
			sig_in1_->get_type(), sig_in1_->get_Ts(), sig_in1_->get_frame_size()));
	}
}


void Block_Fork::initialize( )
{
	v_out_ = new void*[num_outputs_[0]];

	input_type_ = sig_in1_->get_type();

	if(input_type_ == int32)
	{
		v_in_ = get_signal< int32_t >(sig_in1_);
		for(int32_t i=0; i<num_outputs_[0]; ++i)
		{
			v_out_[i] = get_signal< int32_t >(sig_out_[i]);
		}
	}
}


template< typename T >
void Block_Fork::do_fork()
{
#ifndef NDEBUG
		std::cout << " in:  " << *static_cast< const itpp::Vec<T>* >(v_in_) << std::endl;
#endif
		for(int32_t i=0; i<num_outputs_[0]; ++i)
		{
			*static_cast< itpp::Vec<T>* >(v_out_[i]) =
			*static_cast< const itpp::Vec<T>* >(v_in_);
#ifndef NDEBUG
			std::cout << " out: " << *static_cast< itpp::Vec<T>* >(v_out_[i]) << std::endl;
#endif
			assert(*static_cast< itpp::Vec<T>* >(v_out_[i]) ==
				*static_cast< const itpp::Vec<T>* >(v_in_));
		}
}


void Block_Fork::process()
{
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
#endif
	if(input_type_ == int32)
		do_fork< int32_t >();
	else if(input_type_ == real)
		do_fork< real_t >();
	else
		do_fork< complex_t >();
}


Block_Fork::Block_Fork()
{
	set_name(BLOCK_NAME);
	set_description("This is a block for testing purposes. It has 1 inputs and 2 outputs.");
	configure_parameters();
}


Block_Fork::~Block_Fork()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
	if(is_configured())
	{
		delete[] v_out_;
		delete[] sig_out_;
	}
}


ACCESS_FUNCS(Fork)
