#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <itpp/itcomm.h>



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

	// input signals
	const InPort* sig_in_;
	const itpp::cvec *in_vector_;

	// output signals
	OutPort* sig_out_;
	itpp::cvec *out_vector_;

	// block parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;
	real_vec_t alpha_;
	int32_vec_t filter_length_;
	int32_vec_t downsampling_factor_;

	// pulse shaper object
	itpp::Raised_Cosine< complex_t > rc;
	// filter object
	itpp::MA_Filter< complex_t, double, complex_t > mf;
};



HumpBlock::HumpBlock()
{
	set_name("RaisedCosineReceive");
	set_description("Raised cosine receive filter");
}



void HumpBlock::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in", complex, Ts_[0], framesize_[0]));
}



void HumpBlock::setup_output_ports()
{
	sig_out_ = add_port(new OutPort("out", complex,
		sig_in_->get_Ts() * downsampling_factor_[0],
		sig_in_->get_frame_size() / downsampling_factor_[0])
	);
}



void HumpBlock::initialize()
{
	in_vector_ = get_signal< complex_t >(sig_in_);
	out_vector_ = get_signal< complex_t >(sig_out_);

	rc.set_pulse_shape(alpha_[0], filter_length_[0], downsampling_factor_[0]);
	mf.set_coeffs(rc.get_pulse_shape());
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
		(new Parameter(&alpha_, real, "Roll-off factor"))
		->add_constraint(new LessThanConstraint< real_t >(0.0, true))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&filter_length_, int32, "Filter length"))
		->add_constraint(new ModuloConstraint< int32_t >(2))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&downsampling_factor_, int32, "Upsampling factor"))
		->add_constraint(new GreaterThanConstraint< int32_t >(1))
		->add_constraint(new SizeConstraint(1))
	);
}



void HumpBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl << " symbols in: " ;
	std::cout << *in_vector_ << std::endl;
#endif

	*out_vector_ = mf(*in_vector_);

#ifndef NDEBUG
	std::cout << " symbols out: " ;
	std::cout << *out_vector_ << std::endl;
#endif
}


#include "block/create.hpp"
