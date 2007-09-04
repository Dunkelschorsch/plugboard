#include "block.hpp"
#include "types.hpp"
#include "port.hpp"

#define HAS_INPUTS
#define HAS_OUTPUTS

static const std::string BLOCK_NAME = "CHANGE ME!";

/* please replace all occurences of '$NAME' with BLOCK_NAME */

class Block_$NAME : public Block
{
public:

	Block_$NAME();

	~Block_$NAME();

	void wakeup();

private:

	void configure_parameters() __attribute__ ((visibility("hidden")));

#ifdef HAS_INPUTS
	bool setup_input_ports() __attribute__ ((visibility("hidden")));
#endif

#ifdef HAS_OUTPUTS
	bool setup_output_ports() __attribute__ ((visibility("hidden")));
#endif

/* member variable declarations go here */

};


#ifdef HAS_INPUTS
bool Block_$NAME::setup_input_ports()
{
/* calls to "add_port(InPort &) go here */
	return true;
}
#endif


#ifdef HAS_OUTPUTS
bool Block_$NAME::setup_output_ports()
{
/* calls to "add_port(OutPort &) go here */
	return true;
}
#endif


void Block_$NAME::configure_parameters()
{
/* calls to "add_parameter()" go here */
}


void Block_$NAME::wakeup()
{

}


Block_$NAME::Block_$NAME()
{
	name_ = BLOCK_NAME;
	set_description("your description here");
	configure_parameters();
}


Block_$NAME::~Block_$NAME()
{

}

DEFINE_ACCESS_FUNCTIONS($NAME)
