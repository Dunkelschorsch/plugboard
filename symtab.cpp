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

#include <cassert>

#include "symtab.hpp"
#include "variable/variable.hpp"
#include "exception/input.hpp"

#define PB_DEBUG_MESSAGE_COLOUR \033[01;35m
#define PB_DEBUG_MESSAGE_SOURCE Symtab

#include "colour_debug.hpp"

namespace plugboard
{
	Symtab::Symtab(const Symtab* parent) : parent_(parent)
	{
	}


	Symtab::Symtab() : parent_(NULL) { }


	bool Symtab::add_var(const std::string& name, const Variable& v)
	{
		assert(static_cast< bool >(v) == true);

		ht_t::const_iterator it = ht_.find(name);
		if (it != ht_.end())
		{
			return false;
		}
		ht_[name] = v;
		return true;
	}


	const Variable& Symtab::get_var(const std::string& name) const
	{
		PB_DEBUG_MESSAGE("looking up variable '" << name << "'")

		ht_t::const_iterator it = ht_.find(name);

		if (it == ht_.end())
		{
			PB_DEBUG_MESSAGE("cannot find variable '" << name << "'")
			PB_DEBUG_MESSAGE("available variables are:")
			for(ht_t::const_iterator vars=ht_.begin(); vars != ht_.end(); vars++)
			{
				PB_DEBUG_MESSAGE(vars->first)
			}

			if(parent_ != NULL)
			{
				return parent_->get_var(name);
			}
			else
			{
				// checked ok. does not leak memory
				throw UndefinedVariableException(name.c_str());
			}
		}
		return it->second;
	}
} // namespace plugboard
