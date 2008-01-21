#ifndef NDEBUG
#include <iostream>
#endif

#include <vector>
#include <map>
#include <boost/bind.hpp>
#include "exceptions.hpp"
#include "subsystem.hpp"
#include "system.ipp"


using boost::bind;



struct SubsystemImpl : public SystemImpl
{
	std::vector< OutPort* > sig_out_;
	std::vector< InPort* > sig_in_;

	typedef std::pair< std::string, std::string > port_description_t;

	std::map< std::string, port_description_t > exits_;
	std::map< std::string, port_description_t > entries_;

	template< class PortT >
	struct PortAddAction;

	template< class PortT >
	struct ReplacePortAction;
};



void Subsystem::process()
{
#ifndef NDEBUG
	std::cout << "Hello From Subsystem!" << std::endl;
#endif
	wakeup_sys(1);
}



void Subsystem::configure_parameters( )
{

}



Subsystem::~Subsystem( )
{
#ifndef NDEBUG
	std::cout << "Bye from Subystem." << std::endl;
#endif
}



Subsystem::Subsystem() : System(*new SubsystemImpl)
{
}



Subsystem::Subsystem(SubsystemImpl &dd) : System(dd)
{
}



template< class PortT >
struct SubsystemImpl::ReplacePortAction
{
	ReplacePortAction(std::vector< PortT* >& sig, const ExecutionMatrix& exec_m) : sig_(sig), exec_m_(exec_m) { }

	template< typename PortDescT >
	void operator()(const PortDescT& p) const
	{
		typename PortT::store_t::iterator source_port_it;
		typename PortT::store_t *port_list;
		
		std::string gateway_port(p.first);
		std::string block_inside = p.second.first;
		std::string port_source = p.second.second;

		if(not exec_m_.block_exists(block_inside))
		{
			throw InvalidBlockNameException(block_inside);
		}

 		port_list = exec_m_[block_inside]->get_port_list< PortT >();
		// so we found our source block. now let's see if the given port name was valid
		source_port_it =
			std::find_if
			(
				port_list->begin(),
				port_list->end(),
				bind(&PortT::get_name, _1) == port_source
			);

		// unfortunately the given port name was invalid
		if (source_port_it == port_list->end())
		{
			throw InvalidPortNameException(port_source);
		}

		*source_port_it =
			**std::find_if
			(
				sig_.begin(),
				sig_.end(),
				bind(&PortT::get_name, _1) == gateway_port
			);
	}

	std::vector< PortT* >& sig_;
	const ExecutionMatrix& exec_m_;
};



void Subsystem::initialize()
{
	H_D(Subsystem)

	System::initialize(); // initialize() alone is ambigious. it could also mean Block::initialize()

	std::for_each
	(
		d->exits_.begin(),
		d->exits_.end(),
		SubsystemImpl::ReplacePortAction< OutPort >(d->sig_out_, d->exec_m_)
	);

	std::for_each
	(
		d->entries_.begin(),
		d->entries_.end(),
		SubsystemImpl::ReplacePortAction< InPort >(d->sig_in_, d->exec_m_)
	);
}



template< class PortT >
struct SubsystemImpl::PortAddAction
{
	PortAddAction(std::vector< PortT* >& v, Block* const b) : v_(v), b_(b) { };

	template< typename TupleT >
	void operator()(const TupleT& c) const
	{
		PortT* port_tmp = new PortT(c.first, int32, 1, 2);
		v_.push_back(b_->add_port(port_tmp));
#ifndef NDEBUG
		std::cout << "type of subsystem's " << c.first << ": " << int32 << std::endl;
#endif
	}

	std::vector< PortT* >& v_;
	Block * const b_;
};



bool Subsystem::setup_output_ports()
{
	H_D(Subsystem);

	if(d->exits_.size() == 0)
	{
		return false;
	}

	std::for_each
	(
		d->exits_.begin(),
		d->exits_.end(),
		SubsystemImpl::PortAddAction< OutPort >(d->sig_out_, this)
	);

	return true;
}



bool Subsystem::setup_input_ports()
{
	H_D(Subsystem);

	if(d->entries_.size() == 0)
	{
		return false;
	}

	std::for_each
	(
		d->entries_.begin(),
		d->entries_.end(),
		SubsystemImpl::PortAddAction< InPort >(d->sig_in_, this)
	);

	return true;
}



void Subsystem::create_input(const std::string & port_in, const std::string & block_sink, const std::string & port_sink)
{
	H_D(Subsystem)
	d->entries_[port_in] = std::make_pair(block_sink, port_sink);
}



void Subsystem::create_output(const std::string & block_source, const std::string & port_source, const std::string & port_out)
{
	H_D(Subsystem)
	d->exits_[port_out] = std::make_pair(block_source, port_source);
}
