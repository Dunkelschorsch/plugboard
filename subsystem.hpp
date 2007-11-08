#ifndef _SUBSYSTEM_HPP
#define _SUBSYSTEM_HPP

#include "block.hpp"
#include "system.hpp"


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

	bool setup_output_ports();
	bool setup_input_ports();

protected:
	Subsystem(SubsystemImpl &dd);

private:
	H_DECLARE_PRIVATE(Subsystem);
};

#endif // _SUBSYSTEM_HPP
