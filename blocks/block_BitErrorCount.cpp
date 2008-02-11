#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include <iostream>

#include <itpp/comm/error_counters.h>
#include <itpp/itbase.h>
#include <itpp/itcomm.h>

static const std::string BLOCK_NAME = "BitErrorCount";

class Block_BitErrorCount : public Block, public Sink
{
public:

	Block_BitErrorCount();
	~Block_BitErrorCount();

	void initialize();
	void process();

private:
	void configure_parameters() ;
	bool setup_input_ports() ;


	InPort *tx_in_, *rx_in_;
	itpp::BERC *berc;

	const int32_t *tx, *rx;
	itpp::Vec< int32_t > *tx_vec, *rx_vec;

};



Block_BitErrorCount::Block_BitErrorCount()
{
	set_name(BLOCK_NAME);
	set_description("This is a block for testing purposes. It has 2 inputs and 1 outputs.");
	configure_parameters();
}



Block_BitErrorCount::~Block_BitErrorCount()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << BLOCK_NAME << "!" << std::endl;
#endif
	berc->report();
	delete tx_vec;
	delete rx_vec;
	delete berc;
}



void Block_BitErrorCount::configure_parameters() { }



bool Block_BitErrorCount::setup_input_ports()
{
	tx_in_ = add_port(new InPort("tx", int32, 0, 0));
	rx_in_ = add_port(new InPort("rx", int32, 0, 0));
	return true;
}



void Block_BitErrorCount::initialize()
{
	tx = get_data_ptr< int32_t >(tx_in_);
	rx = get_data_ptr< int32_t >(rx_in_);

	tx_vec = new itpp::Vec< int32_t >(const_cast< int* >(tx), tx_in_->get_frame_size(), false);
	rx_vec = new itpp::Vec< int32_t >(const_cast< int* >(rx), tx_in_->get_frame_size(), false);

	berc = new itpp::BERC;
}



void Block_BitErrorCount::process()
{
#ifndef NDEBUG
	std::cout << "Hello from Block_" << BLOCK_NAME << "!" << std::endl;

	std::cout << "rx: " << *rx_vec << std::endl;
	std::cout << "tx: " << *tx_vec << std::endl;
#endif
	berc->count(to_bvec(*tx_vec), to_bvec(*rx_vec));
}



ACCESS_FUNCS(BitErrorCount)
