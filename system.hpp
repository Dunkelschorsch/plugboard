#ifndef _SYSTEM_HPP
#define _SYSTEM_HPP

#include "block.hpp"
#include "variable.hpp"


/** The System class
 */
class System
{
public:

	System();
	~System();

	void add_block(Block* b, const std::string& name_sys);

	void add_variable(const Variable& var);

	void connect_ports(const std::string& block_source, const std::string& port_source,
		const std::string& block_sink, const std::string& port_sink);

	void wakeup_block(const std::string& name, uint32_t times);

private:
	struct SystemImpl;
	SystemImpl *d;
};


#endif // _SYSTEM_HPP
