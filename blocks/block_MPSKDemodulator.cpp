#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <itpp/comm/modulator.h>



class HumpBlock : public Block, public Sink, public Source
{
public:
	HumpBlock();

private:
	void configure_parameters();
	void setup_input_ports();
	void setup_output_ports();

	void process();
	void initialize();

	OutPort* bits_out_;
	void *bits_v_;

	const InPort* symbols_in_;
	const itpp::cvec *symbol_vector_;

	// Input parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;

	int32_vec_t M_;
	int32_vec_t soft_demod_;

	itpp::PSK mod;
};



HumpBlock::HumpBlock()
{
	set_name("MPSKDemodulator");
	set_description("M-Ary PSK Demodulator");
}



void HumpBlock::setup_input_ports()
{
	symbols_in_ = add_port(new InPort("symbols", complex, Ts_[0], framesize_[0]));
}



void HumpBlock::setup_output_ports()
{
	if(soft_demod_[0] == 1)
		bits_out_ = add_port(new OutPort("bits", real, symbols_in_->get_Ts(), static_cast< unsigned int >(log2(M_[0]))*symbols_in_->get_frame_size()));
	else
		bits_out_ = add_port(new OutPort("bits", int32, symbols_in_->get_Ts(), static_cast< unsigned int >(log2(M_[0]))*symbols_in_->get_frame_size()));
}



void HumpBlock::initialize()
{
	if(soft_demod_[0] == 1)
		bits_v_ = get_signal< real_t >(bits_out_);
	else
		bits_v_ = get_signal< int32_t >(bits_out_);

	symbol_vector_ = get_signal< complex_t >(symbols_in_);

	mod = itpp::PSK(M_[0]);
}



void HumpBlock::configure_parameters()
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



void HumpBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl;
	std::cout << " input(" << symbol_vector_->size() << "): " << *symbol_vector_ << std::endl;
	std::cout << " output(";
#endif
	if(soft_demod_[0] == 1)
	{
		mod.demodulate_soft_bits(*symbol_vector_, 1, *static_cast< itpp::Vec< real_t >* >(bits_v_));
#ifndef NDEBUG
		
		std::cout << static_cast< itpp::Vec< real_t >* >(bits_v_)->size() << "): " <<
			*static_cast< itpp::Vec< real_t >* >(bits_v_) << std::endl;
#endif
	}
	else {
		*static_cast< itpp::Vec< int32_t >* >(bits_v_) = to_ivec(mod.demodulate_bits(*symbol_vector_));
#ifndef NDEBUG

		std::cout << static_cast< itpp::Vec< int32_t >* >(bits_v_)->size() << "): " <<
			*static_cast< itpp::Vec< int32_t >* >(bits_v_) << std::endl;
#endif
	}
}


#include "block/create.hpp"
