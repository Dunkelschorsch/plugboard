#include "block/block_ptr.hpp"
#include "block/block.hpp"

#ifndef NDEBUG
#include <iostream>
#endif


int plugboard::intrusive_ptr_add_ref(plugboard::Source const*)
{
	return 0;
}

int plugboard::intrusive_ptr_add_ref(plugboard::Sink const*)
{
	return 0;
}

int plugboard::intrusive_ptr_add_ref(plugboard::Block* b)
{
#ifndef NDEBUG
	std::cout << "intrusive_ptr_add_ref(plugboard::Block*)" << std::endl;
#endif
	return b->add_ref();
}

// release functions
int plugboard::intrusive_ptr_release(plugboard::Source const*)
{
	return 0;
}

int plugboard::intrusive_ptr_release(plugboard::Sink const*)
{
	return 0;
}

int plugboard::intrusive_ptr_release(plugboard::Block* b)
{
#ifndef NDEBUG
	std::cout << "intrusive_ptr_release(plugboard::Block*)" << std::endl;
#endif
	int num_refs = b->release();
	if(num_refs == 0)
	{
		delete b;
	}
	return num_refs;
}
