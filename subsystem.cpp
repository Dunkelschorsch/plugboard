/*!
 * \file
 * \brief
 * \author Armin Schmidt
 *
 * ----------------------------------------------------------------------------
 *
 * PlugBoard - A versatile communication simulation framework
 * Copyright (C) 2007-2008  Armin Schmidt
 *
 * This file is part of PlugBoard.
 *
 * PlugBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PlugBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PlugBoard.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ----------------------------------------------------------------------------
 */

#include <vector>
#include <map>

#ifndef NDEBUG
#include <iostream>
#endif

#include <boost/bind.hpp>

#include "subsystem.hpp"
#include "exception/block.hpp"
#include "exception/port.hpp"
#include "system.ipp"
#include "port/port.hpp"


namespace plugboard
{
	using boost::bind;

	struct SubsystemImpl : public SystemImpl
	{
		std::vector< OutPort* > sig_out;
		std::vector< InPort* > sig_in;

		typedef std::pair< std::string, std::string > port_description_t;

		std::map< std::string, port_description_t > exits;
		std::map< std::string, port_description_t > entries;

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
		ReplacePortAction(std::vector< PortT* >& sig, const ExecutionMatrix& exec_m) : sig(sig), exec_m(exec_m) { }

		template< typename PortDescT >
		void operator()(const PortDescT& p) const
		{
			typename PortT::store_t::iterator source_port_it;
			typename PortT::store_t *port_list;

			std::string gateway_port(p.first);
			std::string block_inside = p.second.first;
			std::string port_source = p.second.second;

			if(not exec_m.block_exists(block_inside))
			{
				throw InvalidBlockNameException(block_inside);
			}

			port_list = exec_m[block_inside]->get_port_list< PortT >();
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
					sig.begin(),
					sig.end(),
					bind(&PortT::get_name, _1) == gateway_port
				);
		}

		std::vector< PortT* >& sig;
		const ExecutionMatrix& exec_m;
	};



	void Subsystem::initialize()
	{
		PB_D(Subsystem)

		System::initialize(); // initialize() alone is ambigious. it could also mean Block::initialize()

		std::for_each
		(
			d->exits.begin(),
			d->exits.end(),
			SubsystemImpl::ReplacePortAction< OutPort >(d->sig_out, d->exec_m)
		);

		std::for_each
		(
			d->entries.begin(),
			d->entries.end(),
			SubsystemImpl::ReplacePortAction< InPort >(d->sig_in, d->exec_m)
		);
	}



	template< class PortT >
	struct SubsystemImpl::PortAddAction
	{
		PortAddAction(std::vector< PortT* >& v, Block* const b) : v(v), b(b) { };

		template< typename TupleT >
		void operator()(const TupleT& c) const
		{
			PortT* port_tmp = new PortT(c.first, int32, 1, 2);
			v.push_back(b->add_port(port_tmp));
#ifndef NDEBUG
			std::cout << "type of subsystem's " << c.first << ": " << int32 << std::endl;
#endif
		}

		std::vector< PortT* >& v;
		Block * const b;
	};



	bool Subsystem::setup_output_ports()
	{
		PB_D(Subsystem);

		if(d->exits.size() == 0)
		{
			return false;
		}

		std::for_each
		(
			d->exits.begin(),
			d->exits.end(),
			SubsystemImpl::PortAddAction< OutPort >(d->sig_out, this)
		);

		return true;
	}



	bool Subsystem::setup_input_ports()
	{
		PB_D(Subsystem);

		if(d->entries.size() == 0)
		{
			return false;
		}

		std::for_each
		(
			d->entries.begin(),
			d->entries.end(),
			SubsystemImpl::PortAddAction< InPort >(d->sig_in, this)
		);

		return true;
	}



	void Subsystem::create_input(const std::string & port_in, const std::string & block_sink, const std::string & port_sink)
	{
		PB_D(Subsystem)
		d->entries[port_in] = std::make_pair(block_sink, port_sink);
	}



	void Subsystem::create_output(const std::string & block_source, const std::string & port_source, const std::string & port_out)
	{
		PB_D(Subsystem)
		d->exits[port_out] = std::make_pair(block_source, port_source);
	}
} // namespace plugboard
