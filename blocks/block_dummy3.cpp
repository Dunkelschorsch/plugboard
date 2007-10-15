#include "block.hpp"
#include <iostream>

#define BLOCK_NAME "Dummy3"

class Block_Dummy3 : public Block
{
public:
	Block_Dummy3();

	~Block_Dummy3();

	void process();
private:

	void configure_parameters() __attribute__ ((visibility("hidden")));
};

void Block_Dummy3::configure_parameters()
{
}

void Block_Dummy3::process()
{
	std::cout << "dummy process call!" << std::endl;
}


Block_Dummy3::Block_Dummy3()
{
	name_ = BLOCK_NAME;
#ifndef NDEBUG
	std::cout << "Dummy3 ctor called" << std::endl;
#endif
}

Block_Dummy3::~Block_Dummy3()
{
#ifndef NDEBUG
	std::cout << "Dummy3 dtor called" << std::endl;
#endif
}

DEFINE_ACCESS_FUNCTIONS(Dummy3)
