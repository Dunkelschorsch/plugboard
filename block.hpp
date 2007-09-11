#ifndef _BLOCK_HPP
#define _BLOCK_HPP

#include "port.hpp"
class Variable;


#define DEFINE_ACCESS_FUNCTIONS(NAME)\
extern "C"\
Block* create()\
{\
return new Block_##NAME;\
}\
extern "C"\
const string_t name()\
{\
	return #NAME;\
}


/** \brief The Block class
 *
 *  A more elaborate description.
 */

class Block
{
friend class System;
friend class SystemImpl;

public:
	Block();
	
	typedef std::pair< void *, type_t > param_t;

	typedef std::vector < Block* > store_t;
	
	virtual void wakeup() = 0;

	std::string get_name() const;

	std::string get_name_sys() const;

	std::string get_description() const;

	bool is_configured() const;

	void set_description(const std::string& description);

	bool set_parameter(const Variable& p);

	InPort::store_t & get_inport_list();

	OutPort::store_t & get_outport_list();

	uint16_t get_num_input_ports() const;

	uint16_t get_num_output_ports() const;

	std::vector< param_t > get_params();

	virtual ~Block();
protected:

	InPort* add_port(const InPort& p);

	OutPort* add_port(const OutPort& p);

	void add_parameter(void *var, type_t t);

	virtual void configure_parameters() = 0;

	std::string name_;

	std::string description_;

	uint16_t param_curr_;

	bool configured_;

	InPort::store_t ports_in_;
	
	OutPort::store_t ports_out_;

	std::vector< param_t > params_;

private:
	virtual bool setup_input_ports();

	virtual bool setup_output_ports();

	void set_name_sys(const std::string& name_sys) __attribute__ ((visibility("hidden")));

	std::string name_sys_;

	uint16_t num_output_ports_;

	uint16_t num_input_ports_;
};

namespace {
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
