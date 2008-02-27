#ifndef BLOCK_BUFFER_ACCESS
#define BLOCK_BUFFER_ACCESS

#include "types/typeinfo.hpp"


namespace
{
	// direct access to the underlying c-array. use only if you must
	template< class SignalT >
	inline const SignalT* get_data(const InPort *p)
	{
		return static_cast< typename typeinfo< SignalT >::vector_type* >(p->get_signal_ptr())->_data();
	}
	
	template< class SignalT >
	inline SignalT* get_data(const OutPort *p)
	{
		return static_cast< typename typeinfo< SignalT >::vector_type* >(p->get_signal_ptr())->_data();
	}

	template< class SignalT >
	inline const typename typeinfo< SignalT >::vector_type* get_signal(const InPort *p)
	{
		return static_cast< const typename typeinfo< SignalT >::vector_type* >(p->get_signal_ptr());
	}

	template< class SignalT >
	inline typename typeinfo< SignalT >::vector_type* get_signal(const OutPort *p)
	{
		return static_cast< typename typeinfo< SignalT >::vector_type* >(p->get_signal_ptr());
	}
}

#endif // BLOCK_BUFFER_ACCESS
