#ifndef _SYSTEM_HPP
#define _SYSTEM_HPP

#include "block.hpp"
#include "variable.hpp"

#include "macros.hpp"

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

	void wakeup_sys(uint32_t times = 1);

protected:
	SystemImpl * const d_ptr;
	System(SystemImpl &dd);

private:
	friend class SystemImpl;

	inline SystemImpl *d_func() { return d_ptr; }
	inline const SystemImpl *d_func() const { return d_ptr; }
};


#endif // _SYSTEM_HPP
