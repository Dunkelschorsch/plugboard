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

#ifndef BLOCK_LOADER_HPP
#define BLOCK_LOADER_HPP

#include "pimpl/pimpl.h"
#include <vector>
#include <string>
#include <boost/pool/detail/singleton.hpp>

namespace plugboard
{
	class Block;

	class BlockLoader : public pimpl< BlockLoader >::pointer_semantics
	{
	typedef boost::details::pool::singleton_default< BlockLoader > singleton;
	friend class boost::details::pool::singleton_default< BlockLoader >;

	public:
		uint32_t load_dir(const std::string& dir, const bool recursive = false);

		const std::vector< std::string > & available_blocks() const;

		Block* new_block(const std::string &name);

		static BlockLoader& instance()
		{
			BlockLoader & bl = singleton::instance();
			return bl;
		}

	private:
		BlockLoader();
	};
}

#endif //BLOCK_LOADER_HPP
