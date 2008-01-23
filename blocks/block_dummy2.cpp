#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include <iostream>

static const std::string BLOCK_NAME = "Dummy2";

#undef  HAS_INPUTS
#define HAS_OUTPUTS


class Block_Dummy2 : public Block, public Source
{
public:

	Block_Dummy2();

	~Block_Dummy2();

	void process();
private:

	void configure_parameters() __attribute__ ((visibility("hidden"))) ;

#ifdef HAS_INPUTS
	bool setup_input_ports() __attribute__ ((visibility("hidden")));
#endif

#ifdef HAS_OUTPUTS
	bool setup_output_ports() __attribute__ ((visibility("hidden")));
#endif

	real_vec_t Ts_;
	int_vec_t framesize_;

	OutPort *sig_out_;
};



bool Block_Dummy2::setup_output_ports()
{
	
	std::cout << "Dummy2 setup_output_ports() called with Ts=" << Ts_[0] << std::endl;
	sig_out_ = add_port(new OutPort("humpOut1", complex, Ts_[0], framesize_[0]));

	return true;
}

void Block_Dummy2::configure_parameters()
{
	add_parameter(&Ts_, real, "Sample time");
	add_parameter(&framesize_, int32, "Frame size");
}

void Block_Dummy2::process()
{
	complex_t *z;
	
	z = get_data_ptr< complex_t >(sig_out_);

	std::fill(z, z+sig_out_->get_frame_size(), 666);

	sig_out_->send();
}


Block_Dummy2::Block_Dummy2()
{
	name_ = BLOCK_NAME;
	set_description("This is a dummy block for testing purposes.");
	configure_parameters();
}

Block_Dummy2::~Block_Dummy2()
{

}

ACCESS_FUNCS(Dummy2)
