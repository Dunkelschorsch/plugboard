#ifndef PORT_OUT_HPP
#define PORT_OUT_HPP

#include "port/port_base.hpp"
#include "message.hpp"
#include "visibility.hpp"

namespace plugboard
{
	class InPort;

	class DSOEXPORT OutPort : public BasePort< OutPort >
	{
	public:
		OutPort(const std::string& name, const type_t type, const real_t Ts, const uint32_t frame_size);
		void connect(InPort& other, uint32_t signal_buffer_id);
		void disconnect(const InPort& other);
		std::tr1::function< Message() > send;
	};
} // namespace plugboard

#endif // PORT_OUT_HPP
