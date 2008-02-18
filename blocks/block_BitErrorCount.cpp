#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "constraint.hpp"
#include <iostream>

#include <itpp/comm/error_counters.h>
#include <itpp/itbase.h>
// #include <itpp/itcomm.h>

static const std::string BLOCK_NAME = "BitErrorCount";

class Block_BitErrorCount : public Block, public Sink
{
public:

	Block_BitErrorCount();
	~Block_BitErrorCount();

	void initialize();
	void process();

private:
	void setup_input_ports();
	void configure_parameters();

	InPort *tx_in_, *rx_in_;
	itpp::Vec< int32_t > *tx_vec, *rx_vec;

	itpp::BERC berc;

	std::vector< std::string > filename_;
};



void Block_BitErrorCount::configure_parameters()
{
	add_parameter
	(
		(new Parameter(&filename_, string, "Filename to save"))
// 		->add_constraint(new SizeConstraint(1))
	);
}



Block_BitErrorCount::Block_BitErrorCount()
{
	set_name(BLOCK_NAME);
	set_description("This is a block for testing purposes. It has 2 inputs and 1 outputs.");
	configure_parameters();
}



Block_BitErrorCount::~Block_BitErrorCount()
{
	std::cout << filename_[0] << std::endl;
	berc.report();
}



void Block_BitErrorCount::setup_input_ports()
{
	tx_in_ = add_port(new InPort("tx", int32, 0, 0));
	rx_in_ = add_port(new InPort("rx", int32, 0, 0));
}



void Block_BitErrorCount::initialize()
{
	tx_vec = get_signal< int32_t >(tx_in_);
	rx_vec = get_signal< int32_t >(rx_in_);

	berc = itpp::BERC();
}



void Block_BitErrorCount::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl;

	std::cout << " rx: " << *rx_vec << std::endl;
	std::cout << " tx: " << *tx_vec << std::endl;
#endif
	berc.count(to_bvec(*tx_vec), to_bvec(*rx_vec));
}



ACCESS_FUNCS(BitErrorCount)
