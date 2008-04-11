#ifndef BLOCK_SINK_HPP
#define BLOCK_SINK_HPP

#include "port/port_in.hpp"
#include "visibility.hpp"

namespace plugboard
{
	template< class PortT > class PortTraits;

	class DSOEXPORT Sink
	{
	friend class PortTraits< InPort >;

	public:
		Sink();

		inline void call_setup_input_ports() { setup_input_ports(); }
		uint16_t get_num_input_ports() const;

		InPort::store_t * get_port_list();
		InPort::store_t * get_port_list() const;

	protected:
		virtual ~Sink();

	private:
		virtual void setup_input_ports() = 0;
		uint16_t num_input_ports_;
		InPort::store_t ports_in_;
	};
} // namespace plugboard

#endif // BLOCK_SINK_HPP
