#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "parameter.hpp"
#include "port/port.hpp"
#include "block/port_traits.hpp"
class Variable;


#include <map>
#include <set>
#include <deque>
#include <vector>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#ifndef NDEBUG
#include <iostream>
#endif



/** \brief The Block class
 *
 *  A more elaborate description.
 */

class Block
{
friend class System;
friend class SystemImpl;
friend class SubsystemImpl;
friend class PortTraits< InPort >;
friend class PortTraits< OutPort >;

public:
	Block();
	virtual ~Block();

	typedef std::deque< Block * > store_t;

	virtual void initialize();

	virtual void process() = 0;

	virtual void advance();

	const std::string& get_name() const
	{
		return name_;
	}

	const std::string& get_name_sys() const
	{
		return name_sys_;
	}

	const std::string& get_description() const
	{
		return description_;
	}

	bool is_configured() const
	{
		return configured_;
	}

	void set_description(const std::string& description)
	{
		description_ = description;
	}

	bool set_parameter(const Variable& p);

	const std::string& get_parameter_description() const
	{
		return params_[param_curr_].get_description();
	}

	type_t get_parameter_type() const
	{
		return params_[param_curr_].get_type();
	}

	const std::vector< Parameter >& get_params() const
	{
		return params_;
	}

	const std::set< std::string >& get_connections() const
	{
		return connected_blocks_;
	}

	uint16_t get_num_input_ports() const
	{
		return num_input_ports_;
	}

	uint16_t get_num_output_ports() const
	{
		return num_output_ports_;
	}

	virtual bool setup_output_ports()
	{
		return false;
	}

	virtual bool setup_input_ports()
	{
		return false;
	}

	template < typename T >
	void copy_parameter(void*, Variable&);

	template< class PortT >
	typename PortT::store_t * get_port_list();

	template< class PortT >
	const typename PortT::store_t * get_port_list() const;

	std::vector< std::pair< OutPort::store_t::iterator, InPort::store_t::iterator > > connect_calls;

protected:

	template< class PortT >
	PortT* add_port(PortT * const p);

	void register_parameter_types();

	virtual void configure_parameters() = 0;

	void add_parameter(void* var, type_t t, std::string description);

	InPort::store_t & get_inport_list();

	OutPort::store_t & get_outport_list();

	typedef std::map< type_t, boost::function< void(void*, Variable&) > > parameter_factory_t;

	parameter_factory_t parameter_factory_;

	std::vector< Parameter > params_;

	uint16_t param_curr_;

	bool configured_;

	std::string name_;

	std::string description_;

	InPort::store_t ports_in_;

	OutPort::store_t ports_out_;

	std::set< std::string > connected_blocks_;

private:
	void set_name_sys(const std::string& name_sys)
	{
		name_sys_ = name_sys;
	}

	void add_connection(const std::string& name)
	{
		connected_blocks_.insert(name);
	}

	std::string name_sys_;

	uint16_t num_output_ports_;

	uint16_t num_input_ports_;
};




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
		return name_exists(p, it, this);
	}

	p->set_owner_block_name(get_name_sys());
	port_list->push_back(*p);
	increment_no_of_ports(p, this);
	delete p;

	return &(port_list->back());
}



template< >
inline InPort::store_t * Block::get_port_list< InPort >()
{
	return &ports_in_;
}



template< >
inline OutPort::store_t * Block::get_port_list< OutPort >()
{
	return &ports_out_;
}



template< >
inline const InPort::store_t * Block::get_port_list< InPort >() const
{
	return &ports_in_;
}



template< >
inline const OutPort::store_t * Block::get_port_list< OutPort >() const
{
	return &ports_out_;
}


#endif // BLOCK_HPP
