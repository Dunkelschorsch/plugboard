#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include "scramble.hpp"


class HumpBlock : public Block, public Source
{
public:
	HumpBlock();

private:
	void process();
	void initialize();
	void configure_parameters();
	void setup_output_ports();

	OutPort *scr_out_;
	itpp::cvec *c_vector_;

	int32_vec_t framesize_;
	real_vec_t Ts_;

	int32_vec_t sequence_length_, sequence_number_, offset_;

	itpp::cvec Z;

	typedef LongUplinkScrambler scramble_t;
	scramble_t s;
};



HumpBlock::HumpBlock()
{
	set_name("UMTS Long Uplink Scrambler");
	set_description("Creates Long UMTS Uplink Scrambling sequences");
}



void HumpBlock::setup_output_ports()
{
	scr_out_ = add_port(new OutPort("out", complex, Ts_[0], framesize_[0]));
}




void HumpBlock::initialize()
{
	c_vector_ = get_signal< complex_t >(scr_out_);

	s = scramble_t(sequence_number_[0], offset_[0]);
	s.generate();
}



void HumpBlock::configure_parameters()
{
	add_parameter
	(
		(new Parameter(&Ts_, real, "Sample Time"))
		->add_constraint(new GreaterThanConstraint< real_t >(0.0))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&framesize_, int32, "Frame Size"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&sequence_length_, int32, "Scrambling sequence length"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&sequence_number_, int32, "Scrambling sequence number"))
		->add_constraint(new LessThanConstraint< int32_t >(0, true))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&offset_, int32, "Offset"))
		->add_constraint(new LessThanConstraint< int32_t >(0, true))
		->add_constraint(new SizeConstraint(1))
	);
}



void HumpBlock::process()
{
	*c_vector_ = s.get_scrambling_sequence();
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
	std::cout << " generated: " << *c_vector_ << std::endl;
#endif
}

#include "block/create.hpp"
