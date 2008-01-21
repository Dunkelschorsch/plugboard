#ifndef _SYSTEM_IPP
#define _SYSTEM_IPP


#include "signal.hpp"
#include "exec_matrix.hpp"
#include "symtab.hpp"


using boost::function;

struct SystemImpl
{
	SystemImpl();
	~SystemImpl();

	void register_basic_types();

	uint32_t create_signal_buffer(type_t type, uint32_t size);

	void set_buffer_ptrs(OutPort& out, InPort& in, Signal *s);

	void linearize(const std::string& block_start);

	void propagate_signal_attributes();

	void execute_row(uint32_t stage_num, uint32_t row_num);

	Signal::store_t signal_buffers_;

	uint32_t signal_buffer_count_;

	typedef std::map< type_t, function< Signal*(size_t) > > signal_assoc_t;

	typedef std::map< type_t, void* (*)(Signal*) > get_buffer_assoc_t;

	signal_assoc_t signal_factory_;

	get_buffer_assoc_t get_buffer_factory_;

	double simulation_time_;

	Symtab symtab_;

	ExecutionMatrix exec_m_;
};

#endif // _SYSTEM_IPP
