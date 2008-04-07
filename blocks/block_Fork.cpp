#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <iostream>
#include <cstdio>



class HumpBlock : public Block, public Source, public Sink
{
public:

	HumpBlock();
	~HumpBlock();

private:
	void setup_input_ports();
	void setup_output_ports();
	void configure_parameters();

	void initialize();
	void process();

	template< typename T >
	void do_fork();

	template< typename T >
	void do_init();

	OutPort **sig_out_;
	const InPort *sig_in1_;

	const void *v_in_;
	void **v_out_;

	int32_vec_t num_outputs_;
	type_t input_type_;

	void (HumpBlock::*proc)();
};


void HumpBlock::configure_parameters( )
{
	add_parameter
	(
		(new Parameter(&num_outputs_, int32, "Number of outputs"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new LessThanConstraint< int32_t >(1000))
		->add_constraint(new SizeConstraint(1))
	);
}


void HumpBlock::setup_input_ports()
{
	sig_in1_ = add_port(new InPort("in"));
}


void HumpBlock::setup_output_ports()
{
	// only reserve that memory once
	if(get_num_output_ports() != num_outputs_[0])
		sig_out_ = new OutPort* [num_outputs_[0]];

	for(int32_t i=0; i<num_outputs_[0]; ++i)
	{
		char portname[6];
		std::sprintf(portname, "out%d", i+1);

		sig_out_[i] = add_port(new OutPort(portname,
			sig_in1_->get_type(), sig_in1_->get_Ts(), sig_in1_->get_frame_size()));
	}
}


template< typename T >
void HumpBlock::do_init()
{
	v_in_ = get_signal< T >(sig_in1_);
	proc = &HumpBlock::do_fork< T >;

	for(int32_t i=0; i<num_outputs_[0]; ++i)
		v_out_[i] = get_signal< T >(sig_out_[i]);
}


void HumpBlock::initialize()
{
	v_out_ = new void* [num_outputs_[0]];
	input_type_ = sig_in1_->get_type();

	if(input_type_ == int32)
		do_init< int32_t >();
	if(input_type_ == real)
		do_init< real_t >();
	if(input_type_ == complex)
		do_init< complex_t >();
}


template< typename T >
void HumpBlock::do_fork()
{
#ifndef NDEBUG
		std::cout << " in:   " << *static_cast< const itpp::Vec<T>* >(v_in_) << std::endl;
#endif
		for(int32_t i=0; i<num_outputs_[0]; ++i)
		{
			*static_cast< itpp::Vec<T>* >(v_out_[i]) =
			*static_cast< const itpp::Vec<T>* >(v_in_);
#ifndef NDEBUG
			std::cout << " out" << i+1 << ": " << *static_cast< itpp::Vec<T>* >(v_out_[i]) << std::endl;
#endif
			assert(*static_cast< itpp::Vec<T>* >(v_out_[i]) ==
				*static_cast< const itpp::Vec<T>* >(v_in_));
		}
}


void HumpBlock::process()
{
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
#endif
	(this->*proc)();
}


HumpBlock::HumpBlock()
{
	set_name("Fork");
	set_description("Clone the input to an arbitrary, user-defined number of outputs.");
}


HumpBlock::~HumpBlock()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << get_name() << "!" << std::endl;
#endif
	if(is_initialized())
		delete[] v_out_;
	if(is_configured())
		delete[] sig_out_;
}


#include "block/create.hpp"
