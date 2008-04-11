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

#include "environment.hpp"
#include <tr1/unordered_map>

using namespace plugboard;

typedef pimpl< Environment >::implementation EnvironmentImpl;


Environment::Environment() : base() { }


template<>
struct pimpl< Environment >::implementation
{
	implementation();

	std::tr1::unordered_map< std::string, boost::any > properties;
};


EnvironmentImpl::implementation() :
	properties()
{
	properties["threading"] = false;
}


const boost::any& Environment::get(const std::string& prop)
{
	return (*this)->properties[prop];
}


Environment& Environment::set(const std::string& prop, boost::any val)
{
	(*this)->properties[prop] = val;
	return *this;
}
