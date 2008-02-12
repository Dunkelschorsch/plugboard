#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <itpp/comm/modulator.h>


static const char* BLOCK_NAME = "MPSKModulator";


class Block_MPSKModulator : public Block, public Sink, public Source
{
public:

	Block_MPSKModulator();

	void process();
	void initialize();

private:
	void configure_parameters();
	void setup_input_ports();
	void setup_output_ports();


	InPort* bits_in_;
	itpp::ivec *bit_vector_;

	OutPort* symbols_out_;
	itpp::cvec *symbol_vector_;

	// Input parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;

	int32_vec_t M_;
	itpp::PSK mod;
};



Block_MPSKModulator::Block_MPSKModulator()
{
	set_name(BLOCK_NAME);
	set_description("M-Ary PSK Modulator");
	configure_parameters();
}



void Block_MPSKModulator::setup_input_ports()
{
	bits_in_ = add_port(new InPort("bits", int32, Ts_[0], framesize_[0]));
}



void Block_MPSKModulator::setup_output_ports()
{
	symbols_out_ = add_port(new OutPort("symbols", complex, bits_in_->get_Ts(), bits_in_->get_frame_size()));
}



void Block_MPSKModulator::initialize()
{
	bit_vector_ = get_signal< int32_t >(bits_in_);
	symbol_vector_ = get_signal< complex_t >(symbols_out_);

	mod = itpp::PSK(M_[0]);
}



void Block_MPSKModulator::configure_parameters()
{
	add_parameter
	(
		(new Parameter(&Ts_, real, "Sample Time"))
		->add_constraint(new LessThanConstraint< real_t >(0.0, true))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&framesize_, int32, "Frame Size"))
		->add_constraint(new LessThanConstraint< int32_t >(0, true))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&M_, int32, "M"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new SizeConstraint(1))
	);
}



void Block_MPSKModulator::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl << " bits: ";
	std::cout << *bit_vector_ << std::endl;
#endif

	mod.modulate(*bit_vector_, *symbol_vector_);

#ifndef NDEBUG
	std::cout << " symobls: ";
	std::cout << *symbol_vector_ << std::endl;
#endif	
}


ACCESS_FUNCS(MPSKModulator)
