#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <itpp/itbase.h>

static const std::string BLOCK_NAME = "randunibitgen";


class Block_randunibitgen : public Block, public Source
{
public:

	Block_randunibitgen();

	void process();
	void initialize();

private:
	void configure_parameters();

	void setup_output_ports();


	OutPort *bits_out_;
	itpp::ivec *i_vector_;

	int32_vec_t framesize_;
	real_vec_t Ts_;
	int32_vec_t hi_, lo_;
};



Block_randunibitgen::Block_randunibitgen()
{
	set_name(BLOCK_NAME);
	set_description("Creates uniformely distributed bits.");
	configure_parameters();
}



void Block_randunibitgen::setup_output_ports()
{
	bits_out_ = add_port(new OutPort("bits", int32, Ts_[0], framesize_[0]));
}



void Block_randunibitgen::initialize()
{
	itpp::RNG_randomize();
	i_vector_ = get_signal< int32_t >(bits_out_);
}



void Block_randunibitgen::configure_parameters()
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
		(new Parameter(&lo_, int32, "Minimum value"))
		->add_constraint(new LessThanConstraint< int32_t >(0, true))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&hi_, int32, "Maximum value"))
		->add_constraint(new LessThanConstraint< int32_t >(0, true))
		->add_constraint(new SizeConstraint(1))
	);
}



void Block_randunibitgen::process()
{
	*i_vector_ = itpp::randi(framesize_[0], lo_[0], hi_[0]);
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
	std::cout << " generated: " << *i_vector_ << std::endl;
#endif
}



ACCESS_FUNCS(randunibitgen)
