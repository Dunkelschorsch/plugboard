#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <itpp/itcomm.h>
#include <itpp/comm/modulator.h>


static const std::string BLOCK_NAME = "ConvolutionalDecoder";



class Block_ConvolutionalDecoder : public Block, public Sink, public Source
{
public:

	Block_ConvolutionalDecoder();

	void process();
	void initialize();

private:

	void configure_parameters();
	void setup_input_ports();
	void setup_output_ports();

	// input signals
	InPort* sig_in_;
	itpp::vec *in_vector_;

	// output signals
	OutPort* sig_out_;
	itpp::ivec *out_vector_;

	// block parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;

	// code parameters
	real_vec_t code_rate_;
	int32_vec_t constraint_length_;
	int32_vec_t code_type_;

	// codec object
	itpp::Convolutional_Code code;
};



Block_ConvolutionalDecoder::Block_ConvolutionalDecoder()
{
	set_name(BLOCK_NAME);
	set_description("Decoder for Convolutional Codes");
	configure_parameters();
}



void Block_ConvolutionalDecoder::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in", real, Ts_[0], framesize_[0]));
}



void Block_ConvolutionalDecoder::setup_output_ports()
{
	sig_out_ = add_port(new OutPort("out", int32, sig_in_->get_Ts(),
		sig_in_->get_frame_size() / static_cast< int32_t >(1/code_rate_[0])
		- constraint_length_[0]+1));
}



void Block_ConvolutionalDecoder::initialize()
{
	in_vector_ = get_signal< real_t >(sig_in_);
	out_vector_ = get_signal< int32_t >(sig_out_);

	code = itpp::Convolutional_Code();
	code.set_method(itpp::Tail);
	
	itpp::CONVOLUTIONAL_CODE_TYPE type = code_type_[0] == 0 ? itpp::MFD : itpp::ODS;
#ifndef NDEBUG
	std::cout << this->get_name_sys() << " parameters: " << std::endl;
	std::cout << "inverse rate: " << static_cast< int32_t >(1/code_rate_[0]) << std::endl;
	std::cout << "constraint length: " << constraint_length_[0] << std::endl;
#endif
	code.set_code(type, static_cast< int32_t >(1/code_rate_[0]), constraint_length_[0]);
}



void Block_ConvolutionalDecoder::configure_parameters()
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
		(new Parameter(&code_rate_, real, "Code Rate"))
		->add_constraint(new GreaterThanConstraint< real_t >(0.0))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&constraint_length_, int32, "Constraint Length"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&code_type_, int32, "Code Type"))
		->add_constraint(new SizeConstraint(1))
	);
}



void Block_ConvolutionalDecoder::process()
{
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
	std::cout << " input: " << *in_vector_ << std::endl;
#endif
	*out_vector_ = itpp::to_ivec(code.decode(*in_vector_));
}



ACCESS_FUNCS(ConvolutionalDecoder)
