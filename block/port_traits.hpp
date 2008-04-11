#ifndef PORT_TRAITS_HPP
#define PORT_TRAITS_HPP

#ifndef NDEBUG
#include <iostream>
#endif

#include "exception/port.hpp"
#include "block/source.hpp"
#include "block/sink.hpp"


namespace plugboard
{
	class Block;
	class OutPort;

	template< class PortT > class PortTraits;

	// specialization for input ports
	template< >
	struct PortTraits< InPort >
	{
		template< class IteratorT, typename PointerT >
		static InPort* name_exists_action(IteratorT it, InPort * const p, const PointerT * const self);

		template< typename PointerT >
		static void increment_no_of_ports(PointerT * const self);
	};


	template< class IteratorT, typename PointerT >
	InPort* PortTraits< InPort >::name_exists_action(IteratorT it, InPort * const p, const PointerT * const self)
	{
		throw DuplicatePortNameException(self->get_name() + "::" + p->get_name());
		return NULL;
	}


	template< typename PointerT >
	void PortTraits< InPort >::increment_no_of_ports(PointerT * const self)
	{
		// this can go unchecked due to the specialization of PortT to InPort
		dynamic_cast< Sink* >(self)->num_input_ports_++;
	}



	// specialization for output ports
	template< >
	struct PortTraits< OutPort >
	{
		template< class IteratorT, typename PointerT >
		static OutPort* name_exists_action(IteratorT it, OutPort * const p, const PointerT * const self);

		template< typename PointerT >
		static void increment_no_of_ports(PointerT* const self);
	};


	template< class IteratorT, typename PointerT >
	OutPort* PortTraits< OutPort >::name_exists_action(IteratorT it, OutPort * const p, const PointerT * const self)
	{
	#ifndef NDEBUG
		std::cout << "    " << self->get_name_sys() <<
			".add_port(OutPort*): setting output port type to: " << p->get_type() << std::endl;
	#endif
		type_t t = p->get_type();
		real_t Ts = p->get_Ts();
		int32_t framesize = p->get_frame_size();

		// propagating a default value is certainly an error
		assert(t != empty);
		assert(Ts > 0.0);
		assert(framesize > 0);

		it->set_type(t);
		it->set_frame_size(framesize);
		it->set_Ts(Ts);

		delete p;
		return &(*it);
	}


	template< typename PointerT >
	void PortTraits< OutPort >::increment_no_of_ports(PointerT* const self)
	{
		// this can go unchecked due to the specialization of PortT to OutPort
		dynamic_cast< Source* >(self)->num_output_ports_++;
	}
} // namespace plugboard

#endif // PORT_TRAITS_HPP
