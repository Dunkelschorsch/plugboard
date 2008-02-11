#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <itpp/itcomm.h>


static const std::string BLOCK_NAME = "AWGNChannel";


class Block_AWGNChannel : public Block, public Sink, public Source
{
public:

	Block_AWGNChannel();
	~Block_AWGNChannel();

	void process();
	void initialize();

private:
	void configure_parameters();
	bool setup_input_ports();
	bool setup_output_ports();

	// input signals
	InPort* sig_in_;
	itpp::cvec *in_vector_;

	// output signals
	OutPort* sig_out_;
	itpp::cvec *out_vector_;

	// block parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;
	real_vec_t noisevar_;

	int32_t framesize;
	real_t Ts;
	real_t noisevar;

	// channel object
	itpp::AWGN_Channel awgn;
};



Block_AWGNChannel::Block_AWGNChannel()
{
	set_name(BLOCK_NAME);
	set_description("Additive White Gaussian Noise Channel");
	configure_parameters();
}



Block_AWGNChannel::~Block_AWGNChannel()
{
}



bool Block_AWGNChannel::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in", complex, Ts_[0], framesize_[0]));
	return true;
}



bool Block_AWGNChannel::setup_output_ports()
{
	sig_out_ = add_port(new OutPort("out", complex, sig_in_->get_Ts(), sig_in_->get_frame_size()));
	return true;
}



void Block_AWGNChannel::initialize()
{
	framesize_[0] = sig_in_->get_frame_size();
	Ts_[0] = sig_in_->get_Ts();

	in_vector_ = get_signal< complex_t >(sig_in_);
	out_vector_ = get_signal< complex_t >(sig_out_);

	awgn.set_noise(noisevar_[0]);
}



void Block_AWGNChannel::configure_parameters()
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



void Block_AWGNChannel::process()
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


ACCESS_FUNCS(AWGNChannel)
