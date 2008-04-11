#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <itpp/itcomm.h>

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
	real_vec_t noisevar_;

	// channel object
	itpp::AWGN_Channel awgn;
};



PlugBoardBlock::PlugBoardBlock()
{
	set_name("AWGNChannel");
	set_description("Additive White Gaussian Noise Channel");
}



void PlugBoardBlock::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in", complex, Ts_[0], framesize_[0]));
}



void PlugBoardBlock::setup_output_ports()
{
	sig_out_ = add_port(new OutPort("out", complex, sig_in_->get_Ts(), sig_in_->get_frame_size()));
}



void PlugBoardBlock::initialize()
{
	in_vector_ = get_signal< complex_t >(sig_in_);
	out_vector_ = get_signal< complex_t >(sig_out_);

	awgn.set_noise(noisevar_[0]);
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
		(new Parameter(&noisevar_, real, "Sigma squared"))
		->add_constraint(new LessThanConstraint< real_t >(0.0, true))
		->add_constraint(new SizeConstraint(1))
	);
}



void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl << " symbols in: " ;
	std::cout << *in_vector_ << std::endl;
#endif

	*out_vector_ = awgn(*in_vector_);

#ifndef NDEBUG
	std::cout << " symbols out: " ;
	std::cout << *out_vector_ << std::endl;
#endif
}


#include "block/create.hpp"
