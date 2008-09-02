#include "block/block_ptr.hpp"
#include "block/block.hpp"


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
	int num_refs = b->release();
	if(num_refs == 0)
	{
		delete b;
	}
	return num_refs;
}
