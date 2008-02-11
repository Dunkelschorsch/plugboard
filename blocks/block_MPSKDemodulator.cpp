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
	~Block_MPSKDemodulator();

	void process();

	void initialize();
private:

	void configure_parameters();
	bool setup_input_ports();
	bool setup_output_ports();


	OutPort* bits_out_;
	int32_t *hard_bits;
	itpp::ivec *hard_bit_vector_;

	real_t *soft_bits;
	itpp::vec *soft_bit_vector_;

	InPort* symbols_in_;
	const complex_t *symbols;
	itpp::cvec *symbol_vector_;

	// Input parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;

	int32_vec_t M_;
	int32_vec_t soft_demod_;
	itpp::PSK *mod;
};



Block_MPSKDemodulator::Block_MPSKDemodulator()
{
	set_name(BLOCK_NAME);
	set_description("M-Ary PSK Demodulator");
	configure_parameters();
}



Block_MPSKDemodulator::~Block_MPSKDemodulator()
{
	if(soft_demod_[0] == 1)
		delete soft_bit_vector_;
	else
		delete hard_bit_vector_;

	delete symbol_vector_;
	delete mod;
}




bool Block_MPSKDemodulator::setup_input_ports()
{
	symbols_in_ = add_port(new InPort("symbols", complex, Ts_[0], framesize_[0]));
	return true;
}



bool Block_MPSKDemodulator::setup_output_ports()
{
	if(soft_demod_[0] == 1)
		bits_out_ = add_port(new OutPort("bits", real, symbols_in_->get_Ts(), symbols_in_->get_frame_size()));
	else
		bits_out_ = add_port(new OutPort("bits", int32, symbols_in_->get_Ts(), symbols_in_->get_frame_size()));
	return true;
}



void Block_MPSKDemodulator::initialize()
{
	if(soft_demod_[0] == 1)
	{
		soft_bits = get_data_ptr< real_t >(bits_out_);
		soft_bit_vector_ = new itpp::Vec< real_t >(soft_bits, symbols_in_->get_frame_size(), false);
	} else {
		hard_bits = get_data_ptr< int32_t >(bits_out_);
		hard_bit_vector_ = new itpp::Vec< int32_t >(hard_bits, symbols_in_->get_frame_size(), false);
	}

	symbols = get_data_ptr< complex_t >(symbols_in_);
	symbol_vector_ = new itpp::Vec< complex_t >(const_cast< complex_t* >(symbols), symbols_in_->get_frame_size(), false);

	mod = new itpp::PSK(M_[0]);
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
	std::cout << get_name_sys() << std::endl << " input: ";
	std::cout << *symbol_vector_ << std::endl;
	std::cout << "output: " << *symbol_vector_ << std::endl;
#endif
	if(soft_demod_[0] == 1)
	{
		mod->demodulate_soft_bits(*symbol_vector_, 1, *soft_bit_vector_);
#ifndef NDEBUG
		std::cout << *soft_bit_vector_ << std::endl;
#endif	
	}
	else {
		mod->demodulate(*symbol_vector_, *hard_bit_vector_);
#ifndef NDEBUG
		std::cout << *hard_bit_vector_ << std::endl;
#endif	
	}
}



ACCESS_FUNCS(MPSKDemodulator)
