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

#include "block_loader.hpp"
#include "factory.hpp"
#include "exception/block.hpp"
#include "environment.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <map>
#include <ltdl.h>

namespace fs = boost::filesystem;

using namespace plugboard;


template < class IdentifierType, class ProductType >
class BlockFactoryError
{
protected:
	ProductType* OnUnknownType(const IdentifierType& id);
};


template< class IdentifierType, class ProductType >
ProductType* BlockFactoryError< IdentifierType, ProductType >::OnUnknownType(const IdentifierType& id)
{
	// checked ok. does not leak memory
	throw InvalidBlockIdException(id);
}


typedef pimpl< BlockLoader >::implementation BlockLoaderImpl;

template<>
struct pimpl< BlockLoader >::implementation
{
	implementation();
	~implementation();

	typedef Block* (*create_block_func_t) (void);
	typedef Factory
		<
			Block,
			std::string,
			create_block_func_t,
			BlockFactoryError
		> block_factory_t;

	block_factory_t f;
	std::vector< std::string > available_blocks;

	std::string plugin_prefix, plugin_ext;
};



BlockLoaderImpl::implementation()
	: f(),
	plugin_prefix("libblock_"),
	plugin_ext(".so")
{
	uint32_t dl_errors = lt_dlinit();
	for(uint32_t i=0; i<dl_errors; i++)
	{
		std::cerr << lt_dlerror() << std::endl;
	}
	if(dl_errors > 0)
	{
		exit(1);
	}
}



BlockLoaderImpl::~implementation()
{
	lt_dlexit();
}



BlockLoader::BlockLoader() : base() { }



uint32_t BlockLoader::load_dir(const std::string& dir, const bool recursive)
{
	uint32_t block_count = 0;
	implementation& impl = **this;

	bool verbose_plugin_load = boost::any_cast< bool >(Environment::instance().get("verbose_plugin_load"));

	fs::directory_iterator block_iter(fs::system_complete(fs::path(dir, fs::native)));
	const fs::directory_iterator last_lib_file;

	for(/*no init*/; block_iter != last_lib_file; ++block_iter)
	{
		if (fs::is_directory(block_iter->status()))
		{
			if (recursive)
				block_count += load_dir((block_iter->path()).directory_string());
			else
				continue;
		}

		std::string block_file_curr = (block_iter->path()).directory_string();

		if (block_file_curr.rfind(impl.plugin_ext) == std::string::npos)
			continue;

		lt_dlhandle module = lt_dlopen(block_file_curr.c_str());

		if(module != NULL)
		{
			if(verbose_plugin_load)
			{
				std::cerr << "Loading module file '" << block_file_curr << "' ... ";
			}

			implementation::create_block_func_t create = 0;

			std::string block_id;
			block_id = block_file_curr.substr(impl.plugin_prefix.length() + block_file_curr.find(impl.plugin_prefix));
			block_id = std::string(block_id.begin(), block_id.end() - impl.plugin_ext.length());

			create = reinterpret_cast< implementation::create_block_func_t >(lt_dlsym(module, "create"));

			if (create)
			{
				if (!impl.f.Register(block_id, create))
				{
					if(verbose_plugin_load)
					{
						std::cerr << "already exists." << std::endl;
					}
					continue;
				}

				if(verbose_plugin_load)
				{
					std::cerr << "ok" << " (id=" << block_id << ")" << std::endl;
				}

				++block_count;
				impl.available_blocks.push_back(block_id);
			}
		} else
		{
			if(verbose_plugin_load)
			{
				std::cerr << lt_dlerror() << " ... ignoring" << std::endl;
			}
		}
	}
	return block_count;
}



Block* BlockLoader::new_block(const std::string& name)
{
	implementation& impl = **this;
	return impl.f.CreateObject(name);
}



const std::vector< std::string > & BlockLoader::available_blocks() const
{
	implementation const& impl = **this;
	return impl.available_blocks;
}
