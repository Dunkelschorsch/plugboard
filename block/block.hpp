#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <deque>
#include <vector>
#include <boost/bind.hpp>

#include "block/port_traits.hpp"
#include "block/source.hpp"
#include "block/sink.hpp"
#include "parameter.hpp"
class Variable;


/** \brief The Block class
 *
 *  A more elaborate description.
 */


class Block
{
struct BlockImpl;

// the subsystem plays some funny games with ports
friend class SubsystemImpl;

public:
	typedef std::deque< Block * > store_t;

	Block();
	virtual ~Block();

	virtual void initialize();
	virtual void process() = 0;
	virtual void advance();

	const std::string& get_name() const;
	const std::string& get_name_sys() const;
	const std::string& get_description() const;

	void set_description(const std::string& description);
	void set_name(const std::string& name);

	bool set_parameter(const Variable& p);
	const std::vector< const Parameter* >& get_params() const;
	const std::string& get_parameter_description() const;
	type_t get_parameter_type() const;

	bool is_configured() const;

	template< class PortT >
	typename PortT::store_t * get_port_list();

	template< class PortT >
	const typename PortT::store_t * get_port_list() const;

	void set_name_sys(const std::string& name_sys);

protected:
	virtual void configure_parameters() = 0;

	void add_parameter(const Parameter*);

	template< class PortT >
	PortT* add_port(PortT * const p);
	
private:
	BlockImpl* d;
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


#endif // BLOCK_HPP
