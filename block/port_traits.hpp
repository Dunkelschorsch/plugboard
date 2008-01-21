#ifndef PORT_TRAITS_HPP
#define PORT_TRAITS_HPP

#include "exceptions.hpp"
class Block;

#ifndef NDEBUG
#include <iostream>
#endif



template< class PortT >
class PortTraits;

template< >
struct PortTraits< OutPort >
{
	template< class IteratorT, typename PointerT >
	OutPort* name_exists_action(IteratorT it, OutPort * const p, const PointerT * const this_p)
	{
#ifndef NDEBUG
		std::cout << "    " << this_p->get_name_sys() << ".add_port(OutPort*): setting output port type to: " << p->get_type() << std::endl;
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
	void increment_no_of_ports(PointerT* const this_p)
	{
		this_p->num_output_ports_++;
	}
};



template< >
struct PortTraits< InPort >
{
	template< class IteratorT, typename PointerT >
	InPort* name_exists_action(IteratorT it, InPort * const p, const PointerT * const this_p)
	{
		throw DuplicatePortNameException(this_p->get_name() + "::" + p->get_name());
		return NULL;
	}

	template< typename PointerT >
	void increment_no_of_ports(PointerT * const this_p)
	{
		this_p->num_input_ports_++;
	}
};



template< class PortT, class IteratorT, typename PointerT >
inline PortT* name_exists(PortT * const p, IteratorT it, const PointerT * const this_p)
{
	return PortTraits< PortT >().name_exists_action(it, p, this_p);
}



template< class PortT, typename PointerT >
inline void increment_no_of_ports(PortT * const, PointerT * const this_p)
{
	PortTraits< PortT >().increment_no_of_ports(this_p);
}


#endif // PORT_TRAITS_HPP
