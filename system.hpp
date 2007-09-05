#ifndef _SYSTEM_HPP
#define _SYSTEM_HPP

#include "macros.hpp"
class Block;
class Variable;

#include <string>

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

	void wakeup_block(const std::string& name, uint32_t times);

protected:
	SystemImpl * const d_ptr;
	System(SystemImpl &dd);

private:
	friend class SystemImpl;

	inline SystemImpl *d_func() { return d_ptr; }
	inline const SystemImpl *d_func() const { return d_ptr; }
};


#endif // _SYSTEM_HPP
