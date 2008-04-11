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
#include <vector>


namespace plugboard
{
	class Variable;
	class ConstraintBase;

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

		Parameter* add_constraint(const ConstraintBase* c);

		std::vector< const ConstraintBase* >& get_constraints();

		bool is_convertible_to(const Variable& v) const;

		bool is_of_same_type_as(const Variable& v) const;

	private:
		void *data_;
		type_t type_;
		std::string description_;
		std::vector< const ConstraintBase* > constraints_;
	};
} // namespace plugboard

#endif // PARAMETER_HPP
