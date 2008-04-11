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

#ifndef SYMTAB_HPP
#define SYMTAB_HPP

#include <map>

namespace plugboard
{
	class Variable;

	class Symtab
	{
	public:
		Symtab();
		Symtab(const Symtab* parent);

		bool add_var(const std::string& name, const Variable& v);
		const Variable& get_var(const std::string& name) const;

	private:
		typedef std::map< const std::string, Variable > ht_t;

		ht_t ht_;
		const Symtab* parent_;
	};
} // namespace plugboard

#endif // SYMTAB_HPP
