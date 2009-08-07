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

#ifndef PARAMETER_HPP
#define PARAMETER_HPP

#include "types/base.hpp"
#include "visibility.hpp"
#include "constraint.hpp"
#include <vector>


namespace plugboard
{
	class Variable;

	class DSOEXPORT Parameter
	{
	public:
		Parameter(void * data, type_t type, const std::string& description);
		Parameter(const Parameter&);
		Parameter& operator=(const Parameter&);
		~Parameter();

		type_t get_type() const;

		void * get_data() const;

		const std::string& get_description() const;

		Parameter* add_constraint(const constraint_ptr);

		std::vector< constraint_ptr >& get_constraints();

		bool is_convertible_to(const Variable&) const;

		bool is_of_same_type_as(const Variable&) const;

	private:
		void *data_;
		type_t type_;
		std::string description_;
		std::vector< constraint_ptr > constraints_;
	public:
		void * proxy;
	};


	template< class T >
	class ParameterTypedProxy
	{
	public:
		ParameterTypedProxy(Parameter * const pp) : pp_(pp) { }

		template< template< class > class ConstraintT >
		ParameterTypedProxy< T >* add_constraint(const T& val, bool reverse=false)
		{
			pp_->add_constraint(constraint_ptr(new ConstraintT< T >(val, reverse)));
			return this;
		}
		
		template< class ConstraintT >
		ParameterTypedProxy< T >* add_constraint(const ConstraintT& vc)
		{
			pp_->add_constraint(constraint_ptr(new ConstraintT(vc)));
			return this;
		}

	private:
		Parameter * const pp_;
	};
} // namespace plugboard

#endif // PARAMETER_HPP
