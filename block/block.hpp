#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <deque>
#include <vector>

#include <boost/bind.hpp>

#include "pimpl/pimpl.h"
#include "block/port_traits.hpp"
#include "block/source.hpp"
#include "block/sink.hpp"
#include "parameter.hpp"
class Variable;



/** \brief The Block class
 *
 *  A more elaborate description.
 */


class DSOEXPORT Block : public pimpl< Block >::pointer_semantics
{
// the subsystem plays some funny games with ports
friend class SubsystemImpl;

public:
	typedef std::deque< Block * > store_t;

	Block();
	virtual ~Block();

	// non-virtual interface
	void call_initialize();
	void call_process();
	void call_advance();
	void call_finalize();
	void call_configure_parameters();

	const std::string& get_name() const;
	const std::string& get_name_sys() const;
	const std::string& get_description() const;

	void set_description(const std::string& description);
	void set_name(const std::string& name);

	bool set_parameter(const Variable& p);
	const std::vector< Parameter* >& get_params() const;
	const std::string& get_parameter_description() const;
	type_t get_parameter_type() const;

	bool is_configured() const;
	bool is_initialized() const;
	void set_initialized();

	template< class PortT >
	typename PortT::store_t * get_port_list();

	template< class PortT >
	const typename PortT::store_t * get_port_list() const;

	void set_name_sys(const std::string& name_sys);

protected:
	void add_parameter(Parameter * const);

	template< class PortT >
	PortT* add_port(PortT * const p);

private:
	// actual interface
	virtual void initialize();
	virtual void process() = 0;
	virtual void advance();
	virtual void finalize();
	virtual void configure_parameters();
};

#endif // BLOCK_HPP
