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

#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <deque>
#include <vector>

#include "pimpl/pimpl.h"
#include "block/source.hpp"
#include "block/sink.hpp"
#include "parameter.hpp"


/** \brief The Block class
 *
 *  A more elaborate description.
 */
namespace plugboard
{
	class Variable;
	class Parameter;

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

		void set_name_sys(const std::string& name_sys);

	protected:
		template< class T >
		ParameterTypedProxy< T >* add_parameter(std::vector< T >* const v, const std::string& description);

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
} // namespace plugboard

#endif // BLOCK_HPP
