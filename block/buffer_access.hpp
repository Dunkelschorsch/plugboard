#ifndef BLOCK_BUFFER_ACCESS
#define BLOCK_BUFFER_ACCESS

namespace
{
	template< class SignalT >
	inline const SignalT* get_data_ptr(const InPort *p)
	{
		return static_cast< const SignalT* >(p->get_buffer_ptr());
	}
	
	template< class SignalT >
	inline SignalT* get_data_ptr(const OutPort *p)
	{
		return static_cast< SignalT* >(p->get_buffer_ptr());
	}
}

#endif // BLOCK_BUFFER_ACCESS
