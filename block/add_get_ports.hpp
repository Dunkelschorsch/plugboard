#ifndef BLOCK_ADD_GET_PORTS_HPP
#define BLOCK_ADD_GET_PORTS_HPP

#include "block/block.hpp"

namespace plugboard
{
	template< class PortT >
	PortT* Block::add_port(PortT * const p)
	{
		typename PortT::store_t *port_list = get_port_list< PortT >();
		typename PortT::store_t::iterator it =
			std::find_if
			(
				port_list->begin(),
				port_list->end(),
				boost::bind(&PortT::get_name, _1) == p->get_name()
			);

		if (it != port_list->end())
		{
			return PortTraits< PortT >::name_exists_action(it, p, this);
		}

		p->set_owner_block_name(get_name_sys());
		port_list->push_back(*p);
		PortTraits< PortT >::increment_no_of_ports(this);
		delete p;

		return &(port_list->back());
	}


	template< >
	inline InPort::store_t * Block::get_port_list< InPort >()
	{
		return dynamic_cast< Sink* >(this)->get_port_list();
	}


	template< >
	inline const InPort::store_t * Block::get_port_list< InPort >() const
	{
		return dynamic_cast< const Sink* >(this)->get_port_list();
	}


	template< >
	inline OutPort::store_t * Block::get_port_list< OutPort >()
	{
		return dynamic_cast< Source* >(this)->get_port_list();
	}


	template< >
	inline const OutPort::store_t * Block::get_port_list< OutPort >() const
	{
		return dynamic_cast< const Source* >(this)->get_port_list();
	}
} // namespace plugboard

#endif // BLOCK_ADD_GET_PORTS_HPP
