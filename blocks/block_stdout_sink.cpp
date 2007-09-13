#include "block.hpp"
#include "types.hpp"
#include <iostream>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
using namespace boost::lambda;

#define HAS_INPUTS
#undef HAS_OUTPUTS


static const std::string BLOCK_NAME = "stdout_sink";


class Block_stdout_sink : public Block
{
public:

	Block_stdout_sink();

	~Block_stdout_sink();

	void wakeup();

private:

	void configure_parameters() __attribute__ ((visibility("hidden")));

#ifdef HAS_INPUTS
	bool setup_input_ports() __attribute__ ((visibility("hidden")));
#endif

#ifdef HAS_OUTPUTS
	bool setup_output_ports() __attribute__ ((visibility("hidden")));
#endif

	InPort *sig_in_;
};


Block_stdout_sink::Block_stdout_sink()
{
	name_ = BLOCK_NAME;
	configure_parameters();
}


void Block_stdout_sink::configure_parameters()
{

}


#ifdef HAS_INPUTS
bool Block_stdout_sink::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in", empty, 0, 0));
	return true;
}
#endif


#ifdef HAS_OUTPUTS
bool Block_stdout_sink::setup_output_ports()
{
	/* calls to "add_port(OutPort &) go here */
	return true;
}
#endif


void Block_stdout_sink::wakeup()
{
	const complex_t* z;

	z = get_data_ptr< complex_t >(sig_in_);

	std::for_each(z, z+sig_in_->get_frame_size(), std::cout << _1 << " ");
	std::cout << std::endl;
}


Block_stdout_sink::~Block_stdout_sink()
{

}

DEFINE_ACCESS_FUNCTIONS(stdout_sink)
