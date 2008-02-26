#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include <iostream>



class HumpBlock : public Block, public Sink
{
public:
	HumpBlock();

	void process();
	void initialize();
private:
	void setup_input_ports();

	template< typename T >
	void do_display();

	const InPort *sig_in_;
	const void *input_;
};


void HumpBlock::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in"));
}


void HumpBlock::initialize( )
{
	if(sig_in_->get_type() == int32)
		input_ = get_signal< int32_t >(sig_in_);
	else if(sig_in_->get_type() == real)
		input_ = get_signal< real_t >(sig_in_);
	else
		input_ = get_signal< complex_t >(sig_in_);
}


template< typename T >
void HumpBlock::do_display()
{
	std::cout << *static_cast< const itpp::Vec<T>* >(input_) << std::endl;
}


void HumpBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl;
#endif
	std::cout << " ";
	if(sig_in_->get_type() == int32)
		do_display< int32_t >();
	else if(sig_in_->get_type() == real)
		do_display< real_t >();
	else
		do_display< complex_t >();
}


HumpBlock::HumpBlock()
{
	set_name("StdoutSink");
	set_description("Display input signal on standard output.");
	configure_parameters();
#ifndef NDEBUG
	std::cout << "Contructed Block_" << get_name() << "!" << std::endl;
#endif
}


#include "block/create.hpp"
