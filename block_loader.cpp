#include "block_loader.hpp"
#include "exceptions.hpp"

#include <glibmm.h>

#include <iostream>


uint32_t BlockLoader::load_dir(const std::string& dir, const bool recursive)
{
	uint32_t block_count = 0;

	fs::path block_path(fs::initial_path());
	block_path = fs::system_complete(fs::path(dir, fs::native));

	const fs::directory_iterator last_lib_file;
	fs::directory_iterator block_iter(block_path);

	for(/*no init*/; block_iter != last_lib_file; ++block_iter)
	{
		if (fs::is_directory(*block_iter))
		{
			if (recursive)
			{
				block_count += load_dir((block_path/block_iter->leaf()).native_directory_string());
			}
			else
			{	
				continue;
			}
		}
		std::string block_file_curr = (block_path/block_iter->leaf()).native_directory_string();

		if (block_file_curr.rfind(".so") == std::string::npos)
		{
			continue;
		}

		Glib::Module module(block_file_curr, ~Glib::MODULE_BIND_LAZY);
		
		if(module)
		{
			std::cout << "loading module file '" << block_file_curr << "' ... ";
			create_block_func_t create = 0;
			get_block_name_func_t name = 0;
			bool found = false;
	
			found  = module.get_symbol("create", reinterpret_cast<void*&>(create));
			found &= module.get_symbol("name", reinterpret_cast<void*&>(name));

			if (found)
			{
				const std::string block_id = name();
				block_factory_t::const_iterator name_cur = f_.find(block_id);
				if (name_cur != f_.end())
				{
					std::cerr << "already exists." << std::endl;
					continue;
				}

				std::cout << "ok" << " (id=" << block_id << ")" << std::endl;

				++block_count;
				f_[block_id] = create;
				module.make_resident();
			}
		} else
		{
			std::cerr << Glib::Module::get_last_error() << " ... ignoring" << std::endl;
		}
	}
	return block_count;
}


Block* BlockLoader::new_block(const std::string& name)
{
	block_factory_t::const_iterator name_cur = f_.find(name);
	if (name_cur == f_.end())
	{
		throw non_existant_block_error(name);
	}
	return f_[name]();
}
