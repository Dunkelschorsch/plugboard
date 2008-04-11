#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>
#include "macros.hpp"

namespace plugboard
{
	class Block;
	class Variable;
	struct SystemImpl;

	class System
	{
	public:
		System();
		virtual ~System();

		void add_block(Block * const b, const std::string& name_sys);

		void add_variable(const std::string& name, const Variable& var);

		const Variable& get_variable(const std::string& name) const;

		void connect_ports(const std::string& block_source, const std::string& port_source,
			const std::string& block_sink, const std::string& port_sink);

		void initialize();

		void wakeup_sys(uint32_t times = 1);

		void finalize();

	protected:
		PB_DECLARE_PROTECTED(System)

	private:
		PB_DECLARE_PRIVATE(System)
	};
} // namespace plugboard

#endif // SYSTEM_HPP
