#include "block.hpp"
#include "types.hpp"

#define NEED_INIT
#define NEED_ADVANCE

#define HAS_INPUTS
#define HAS_OUTPUTS

static const std::string BLOCK_NAME = "CHANGE ME!";

// please replace all occurences of '$NAME' with BLOCK_NAME

class Block_$NAME : public Block
{
public:

	Block_$NAME();
	~Block_$NAME();

#ifdef NEED_INIT
	void initialize();
#endif

	void process();

#ifdef NEED_ADVANCE
	void advance();
#endif

private:

	void configure_parameters() __attribute__ ((visibility("hidden")));

#ifdef HAS_INPUTS
	bool setup_input_ports() __attribute__ ((visibility("hidden")));
#endif

#ifdef HAS_OUTPUTS
	bool setup_output_ports() __attribute__ ((visibility("hidden")));
#endif

	// member variable declarations go here

};



Block_$NAME::Block_$NAME()
{
	name_ = BLOCK_NAME;
	set_description("your description here");
	configure_parameters();
}



Block_$NAME::~Block_$NAME()
{
	// if you dynamically allocate memory in the constructor (or somewhere else),
	// this is a great place to free it.
}



#ifdef HAS_INPUTS
bool Block_$NAME::setup_input_ports()
{
	// calls to "add_port(InPort *) go here
	return true;
}
#endif



#ifdef HAS_OUTPUTS
bool Block_$NAME::setup_output_ports()
{
	// calls to "add_port(OutPort *) go here
	return true; 
}
#endif



#ifdef NEED_INIT
void Block_$NAME::initialize()
{
	// generic init stuff goes here
}
#endif



void Block_$NAME::configure_parameters()
{
	// calls to "add_parameter()" go here
}



void Block_$NAME::process()
{

}


#ifdef NEED_ADVANCE
void Block_$NAME::advance()
{
	// actions to be carried out in fast forward state go here
}
#endif


DEFINE_ACCESS_FUNCTIONS($NAME)
