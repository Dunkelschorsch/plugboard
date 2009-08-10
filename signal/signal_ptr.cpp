#include "signal/signal.hpp"
#include <boost/checked_delete.hpp>

namespace plugboard
{
	int intrusive_ptr_release(Signal* s)
	{
		int num_ref = s->release();
		if(num_ref == 0)
		{
			boost::checked_delete(s);
		}
		return num_ref;
	}

	int intrusive_ptr_add_ref(Signal* s)
	{
		return s->add_ref();
	}
}
