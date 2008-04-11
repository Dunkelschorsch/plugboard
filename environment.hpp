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

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <string>
#include <boost/any.hpp>
#include <boost/pool/detail/singleton.hpp>
#include "pimpl/pimpl.h"

namespace plugboard
{
	class Environment : public pimpl< Environment >::pointer_semantics
	{
	typedef boost::details::pool::singleton_default< Environment > singleton;
	friend class boost::details::pool::singleton_default< Environment >;

	public:
		static Environment& instance()
		{
			Environment& self = singleton::instance();
			return self;
		}

		const boost::any& get(const std::string&);

		Environment& set(const std::string&, boost::any);

	private:
		Environment();
	};
} // namespace plugboard

#endif // ENVIRONMENT_HPP
