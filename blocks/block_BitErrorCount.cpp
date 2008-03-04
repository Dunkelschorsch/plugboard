#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "constraint.hpp"
#include <iostream>

#include <itpp/comm/error_counters.h>
#include <itpp/itbase.h>



class HumpBlock : public Block, public Sink
{
public:

	HumpBlock();
	~HumpBlock();

private:
	void setup_input_ports();
	void configure_parameters();

	void initialize();
	void process();

	const InPort *tx_in_, *rx_in_;
	const itpp::Vec< int32_t > *tx_vec, *rx_vec;

	itpp::BERC berc;

	std::vector< std::string > filename_;
};



void HumpBlock::configure_parameters()
{
	add_parameter
	(
		(new Parameter(&filename_, string, "Filename to save"))
		->add_constraint(new SizeConstraint(1))
	);
}



HumpBlock::HumpBlock()
{
	set_name("BitErrorCount");
	set_description("Counts bit errors between the two input signals");
}



HumpBlock::~HumpBlock()
{
#ifndef NDEBUG
	std::cout << filename_[0] << std::endl;
#endif
	berc.report();
}



void HumpBlock::setup_input_ports()
{
	tx_in_ = add_port(new InPort("tx", int32, 0, 0));
	rx_in_ = add_port(new InPort("rx", int32, 0, 0));
}



void HumpBlock::initialize()
{
	tx_vec = get_signal< int32_t >(tx_in_);
	rx_vec = get_signal< int32_t >(rx_in_);

	berc = itpp::BERC();
}



void HumpBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl;

	std::cout << " rx: " << *rx_vec << std::endl;
	std::cout << " tx: " << *tx_vec << std::endl;
#endif
	berc.count(to_bvec(*tx_vec), to_bvec(*rx_vec));
}


#include "block/create.hpp"
