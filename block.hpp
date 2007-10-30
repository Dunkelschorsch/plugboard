#ifndef _BLOCK_HPP
#define _BLOCK_HPP

#include "parameter.hpp"
#include "port.hpp"
class Variable;

#include <map>
#include <set>
#include <boost/function.hpp>


#define DEFINE_ACCESS_FUNCTIONS(NAME)	\
extern "C"				\
Block* create()				\
{					\
	return new Block_##NAME;	\
}					\
extern "C"				\
const string_t name()			\
{					\
	return #NAME;			\
}


/** \brief The Block class
 *
 *  A more elaborate description.
 */

class Block
{
friend class SystemImpl;
friend class System;

public:
	Block();
	virtual ~Block();

	typedef std::deque< Block * > store_t;

	virtual void initialize();

	virtual void process() = 0;

	virtual void advance();

	const std::string& get_name() const;

	const std::string& get_name_sys() const;

	const std::string& get_description() const;

	bool is_configured() const;

	void set_description(const std::string& description);

	bool set_parameter(const Variable& p);

	const std::string& get_parameter_description() const;

	type_t get_parameter_type() const;

	const std::vector< Parameter >& get_params() const;

	InPort::store_t & get_inport_list();

	OutPort::store_t & get_outport_list();

	const std::set< std::string >& get_connections() const;

	uint16_t get_num_input_ports() const;

	uint16_t get_num_output_ports() const;

	template < typename T >
	void copy_parameter(void*, Variable&);

	virtual bool setup_output_ports();

	virtual bool setup_input_ports();

	std::vector< std::pair< OutPort::store_t::iterator, InPort::store_t::iterator > > connect_calls;

protected:
	InPort* add_port(InPort * const p);
	OutPort* add_port(OutPort * const p);

	void register_parameter_types();

	virtual void configure_parameters() = 0;

	void add_parameter(void *var, type_t t, std::string description);

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
	void set_name_sys(const std::string& name_sys);

	void add_connection(const std::string& name);

	std::string name_sys_;

	uint16_t num_output_ports_;

	uint16_t num_input_ports_;
};



namespace
{
	template< class T >
	inline const T* get_data_ptr(const InPort *p)
	{
		return static_cast< const T* >(p->get_buffer_ptr());
	}
	
	template< class T >
	inline T* get_data_ptr(const OutPort *p)
	{
		return static_cast< T* >(p->get_buffer_ptr());
	}
}

#endif //_BLOCK_HPP
