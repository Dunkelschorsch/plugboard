#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <itpp/itcomm.h>
#include <itpp/itstat.h>


using namespace plugboard;

class PlugBoardBlock : public Block, public Sink, public Source
{
public:
	PlugBoardBlock();

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
	int32_vec_t upsampling_factor_;

	// pulse shaper object
	itpp::Root_Raised_Cosine< complex_t > rc;
};



PlugBoardBlock::PlugBoardBlock()
{
	set_name("RaisedCosineTransmit");
	set_description("Raised cosine transmit filter");
}



void PlugBoardBlock::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in", complex, Ts_[0], framesize_[0]));
}



void PlugBoardBlock::setup_output_ports()
{
	sig_out_ = add_port(new OutPort("out", complex,
		sig_in_->get_Ts() / upsampling_factor_[0],
		sig_in_->get_frame_size() * upsampling_factor_[0])
	);
}



void PlugBoardBlock::initialize()
{
	in_vector_ = get_signal< complex_t >(sig_in_);
	out_vector_ = get_signal< complex_t >(sig_out_);

	rc.set_pulse_shape(alpha_[0], filter_length_[0], upsampling_factor_[0]);
}



void PlugBoardBlock::configure_parameters()
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
		(new Parameter(&upsampling_factor_, int32, "Upsampling factor"))
		->add_constraint(new GreaterThanConstraint< int32_t >(1))
		->add_constraint(new SizeConstraint(1))
	);
}



void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl << " samples in(" << in_vector_->size() << "): " ;
	std::cout << "rms: " << sqrt(itpp::mean(itpp::pow(itpp::abs(*in_vector_), 2))) << " ";
	std::cout << *in_vector_ << std::endl;
#endif

	*out_vector_ = rc.shape_symbols(*in_vector_);

#ifndef NDEBUG
	std::cout << " samples out(" << out_vector_->size() << "): " ;
	std::cout << "rms: " << sqrt(itpp::mean(itpp::pow(itpp::abs(*out_vector_), 2))) << " ";
	std::cout << *out_vector_ << std::endl;
#endif
}


#include "block/create.hpp"
