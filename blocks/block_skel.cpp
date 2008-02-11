#include "block/block.hpp"
#include "block/create.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"



static const std::string BLOCK_NAME = "CHANGE ME!";

// IMPORTANT!!!
// replace all occurences of '$NAME' with BLOCK_NAME


// must additionally inherit from "public Source" or "public Sink" (or both, of course)
class Block_$NAME : public Block
{
public:

	Block_$NAME();
	~Block_$NAME();

	void process();

// 	void initialize();
// 	void advance();
private:

	void configure_parameters();

// 	at least one of these two must be defined, depending from which classes
// 	(Source, Sink) this Block inherits:

// 	bool setup_input_ports();
// 	bool setup_output_ports();

// 	member variable declarations go here:
	InPort* bits_in_;
};



Block_$NAME::Block_$NAME()
{
	set_name(BLOCK_NAME);
	set_description("your description here");
	configure_parameters();
}



Block_$NAME::~Block_$NAME()
{
	// if you dynamically allocate memory in the constructor (or somewhere else),
	// this is a great place to free it.
}




// bool Block_$NAME::setup_input_ports()
// {
// 	// calls to "add_port(InPort *) go here
// 	return true;
// }



// bool Block_$NAME::setup_output_ports()
// {
// 	// calls to "add_port(OutPort *) go here
// 	return true; 
// }



// void Block_$NAME::initialize()
// {
// 	// generic init stuff goes here
// }



void Block_$NAME::configure_parameters()
{
	// calls to "add_parameter()" go here
}



void Block_$NAME::process()
{

}



// void Block_$NAME::advance()
// {
// 	// actions to be carried out in fast forward state go here
// }



ACCESS_FUNCS($NAME)
