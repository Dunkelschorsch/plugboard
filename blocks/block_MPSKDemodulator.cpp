#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <itpp/comm/modulator.h>


static const std::string BLOCK_NAME = "MPSKDemodulator";


class Block_MPSKDemodulator : public Block, public Sink, public Source
{
public:

	Block_MPSKDemodulator();

	void process();
	void initialize();

private:

	void configure_parameters();
	void setup_input_ports();
	void setup_output_ports();


	OutPort* bits_out_;
	itpp::ivec *hard_bit_vector_;

	itpp::vec *soft_bit_vector_;

	InPort* symbols_in_;
	itpp::cvec *symbol_vector_;

	// Input parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;

	int32_vec_t M_;
	int32_vec_t soft_demod_;
	itpp::PSK mod;
};



Block_MPSKDemodulator::Block_MPSKDemodulator()
{
	set_name(BLOCK_NAME);
	set_description("M-Ary PSK Demodulator");
	configure_parameters();
}



void Block_MPSKDemodulator::setup_input_ports()
{
	symbols_in_ = add_port(new InPort("symbols", complex, Ts_[0], framesize_[0]));
	
}



void Block_MPSKDemodulator::setup_output_ports()
{
	if(soft_demod_[0] == 1)
		bits_out_ = add_port(new OutPort("bits", real, symbols_in_->get_Ts(), symbols_in_->get_frame_size()));
	else
		bits_out_ = add_port(new OutPort("bits", int32, symbols_in_->get_Ts(), symbols_in_->get_frame_size()));
	
}



void Block_MPSKDemodulator::initialize()
{
	if(soft_demod_[0] == 1)
	{
		soft_bit_vector_ = get_signal< real_t >(bits_out_);
	} else {
		hard_bit_vector_ = get_signal< int32_t >(bits_out_);
	}

	symbol_vector_ = get_signal< complex_t >(symbols_in_);

	mod = itpp::PSK(M_[0]);
}



void Block_MPSKDemodulator::configure_parameters()
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

	add_parameter
	(
		(new Parameter(&soft_demod_, int32, "Soft Demodulation"))
		->add_constraint(new LessThanConstraint< int32_t >(0, true))
		->add_constraint(new SizeConstraint(1))
	);
}



void Block_MPSKDemodulator::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl;
	std::cout << " input: " << *symbol_vector_ << std::endl;
	std::cout << " output: ";
#endif
	if(soft_demod_[0] == 1)
	{
		mod.demodulate_soft_bits(*symbol_vector_, 1, *soft_bit_vector_);
#ifndef NDEBUG
		std::cout << *soft_bit_vector_ << std::endl;
#endif
	}
	else {
		mod.demodulate(*symbol_vector_, *hard_bit_vector_);
#ifndef NDEBUG
		std::cout << *hard_bit_vector_ << std::endl;
#endif
	}
}



ACCESS_FUNCS(MPSKDemodulator)
