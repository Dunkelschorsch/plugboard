#ifndef SIGNAL_SIGNAL_PTR_HPP
#define SIGNAL_SIGNAL_PTR_HPP

#include <boost/intrusive_ptr.hpp>
#include "visibility.hpp"

namespace plugboard
{
	class Signal;
	typedef boost::intrusive_ptr< Signal > signal_ptr;

	int intrusive_ptr_add_ref(Signal*) DSOEXPORT;
	int intrusive_ptr_release(Signal*) DSOEXPORT;
}

#endif // SIGNAL_SIGNAL_PTR_HPP
