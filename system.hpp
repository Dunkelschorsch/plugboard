#ifndef _SYSTEM_HPP
#define _SYSTEM_HPP


#include <string>
#include "macros.hpp"
class Block;
class Variable;


/** The System class
 */
struct SystemImpl;
class System
{
public:
	System();
	virtual ~System();

	void add_block(Block* b, const std::string& name_sys);

	void add_variable(const Variable& var);

	void connect_ports(const std::string& block_source, const std::string& port_source,
		const std::string& block_sink, const std::string& port_sink);

	void initialize();

	void wakeup_sys(uint32_t times = 1);

protected:
	H_DECLARE_PROTECTED(System);

private:
	H_DECLARE_PRIVATE(System);
};

#endif // _SYSTEM_HPP
