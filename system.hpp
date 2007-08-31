#ifndef _SYSTEM_HPP
#define _SYSTEM_HPP

#include "symtab.hpp"
#include "block.hpp"
#include "variable.hpp"
#include "exceptions.hpp"
#include "signal.hpp"
#include "port.hpp"

#include <boost/bind.hpp>

/*
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/casts.hpp>
#include <boost/any.hpp>
*/

#include <boost/ptr_container/ptr_deque.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <iostream>

/*
#include <boost/statechart/event.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>

namespace sc = boost::statechart;
*/


/** The System class
 */
class System
{
public:

	System();

	void add_block(Block* b, const std::string& name_sys); // may throw

	void add_variable(const Variable& var);

	void connect_ports(const std::string& block_source, const std::string& port_source,
		const std::string& block_sink, const std::string& port_sink);


	void wakeup_block(const std::string& name, uint32_t times);

	~System();

private:
	uint32_t create_signal_buffer(type_t type, uint32_t size);

	Block::store_t blocks_;

	Signal::store_t signal_buffers_;

	uint32_t signal_buffer_count_;

	Symtab symtab_;

	double simulation_time;
};

#endif // _SYSTEM_HPP
