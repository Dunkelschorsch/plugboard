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

#include "parameter.hpp"
#include "variable/variable.hpp"
#include "constraint.hpp"


namespace plugboard
{
	Parameter::Parameter(void * data, type_t type, const std::string & description) :
		data_(data),
		type_(type),
		description_(description),
		constraints_(),
		proxy(NULL)
	{
	}


	type_t Parameter::get_type() const
	{
		return type_;
	}


	bool Parameter::is_of_same_type_as(const Variable & v) const
	{
		return this->get_type() == v.get_type();
	}


	bool Parameter::is_convertible_to(const Variable & v) const
	{
	  type_t target_type, given_type;
	  target_type = this->get_type();
	  given_type = v.get_type();

	  if(target_type == given_type)
	  {
	    return true;
	  }

	  if(given_type == complex || given_type == string)
	  {
	    return false;
	  }
	  return true;
	}


	void * Parameter::get_data() const
	{
		return data_;
	}


	const std::string & Parameter::get_description() const
	{
		return description_;
	}


	Parameter* Parameter::add_constraint(const constraint_ptr c)
	{
		this->constraints_.push_back(c);
		return this;
	}


	Parameter::~Parameter( )
	{
		if(not (proxy == NULL))
			free(proxy);
	}


	std::vector< constraint_ptr >& Parameter::get_constraints( )
	{
		return constraints_;
	}
} // namespace plugboard
