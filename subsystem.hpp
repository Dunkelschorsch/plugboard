#ifndef SUBSYSTEM_HPP
#define SUBSYSTEM_HPP

#include "block/block.hpp"
#include "system.hpp"

namespace plugboard
{
	class SubsystemImpl;

	class Subsystem : public System, public Block
	{
	public:
		Subsystem();
		~Subsystem();

		void configure_parameters();
		void initialize();
		void process();

		void create_output(const std::string & block_source, const std::string & port_source, const std::string & port_out);

		void create_input(const std::string & port_in, const std::string & block_sink, const std::string & port_sink);

		bool setup_output_ports();
		bool setup_input_ports();

	protected:
		Subsystem(SubsystemImpl &dd);

	private:
		PB_DECLARE_PRIVATE(Subsystem);
	};
} // namespace plugboard

#endif // SUBSYSTEM_HPP
