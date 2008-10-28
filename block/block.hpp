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
#include "visibility.hpp"
#include "types/base.hpp"
#include "block/block_ptr.hpp"

#ifndef NDEBUG
#include <iostream>
#endif

/** \brief The Block class
 *
 *  A more elaborate description.
 */
namespace plugboard
{
	class Block;
	class Variable;
	class Parameter;
	template< class T >
	class ParameterTypedProxy;

	class DSOEXPORT Block : public pimpl< Block >::pointer_semantics
	{
	// the subsystem plays some funny games with ports
	friend class SubsystemImpl;

	// function for intrusive pointer reference counting and destruction
	friend int plugboard::intrusive_ptr_add_ref(Block*);
	friend int plugboard::intrusive_ptr_release(Block*);

	public:
		typedef std::deque< block_ptr > store_t;
		typedef boost::shared_ptr< Parameter > param_ptr;

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

		void set_description(const std::string&);
		void set_name(const std::string&);

		bool set_parameter(const Variable& p);
		const std::vector< param_ptr >& get_params() const;
		const std::string& get_parameter_description() const;
		type_t get_parameter_type() const;

		bool is_configured() const;
		bool is_initialized() const;
		void set_initialized();

		template< class PortT >
		typename PortT::store_t * get_port_list();

		void set_name_sys(const std::string&);

	protected:
		template< class T >
		ParameterTypedProxy< T >* add_parameter(std::vector< T >* const, const std::string&);

		template< class PortT >
		PortT* add_port(PortT * const);

	private:
		// actual interface
		virtual void initialize();
		virtual void process() = 0;
		virtual void advance();
		virtual void finalize();
		virtual void configure_parameters();

		// reference counting
		int add_ref();
		int release();
	};

#ifndef NDEBUG
	template< class T >
	void print_vector_with_length(const std::string& name, const T* vec)
	{
		std::cout << " " << name << "(" << vec->length() << "): " << *vec << std::endl;
	}
#endif
} // namespace plugboard

#endif // BLOCK_HPP
