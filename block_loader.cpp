#include "block_loader.hpp"
#include "factory.hpp"
#include "exceptions.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <map>
#include <ltdl.h>


namespace fs = boost::filesystem;


template<>
struct pimpl< BlockLoader >::implementation
{
	implementation();
	~implementation();

	typedef Block* (*create_block_func_t) (void);
	typedef const std::string (*get_block_name_func_t) (void);
	typedef Factory
		<
			Block,
			std::string,
			create_block_func_t,
			BlockFactoryError
		> block_factory_t;

	block_factory_t f_;
	std::vector< std::string > available_blocks_;
};



pimpl< BlockLoader >::implementation::implementation() : f_()
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



pimpl< BlockLoader >::implementation::~implementation()
{
	lt_dlexit();
}



BlockLoader::BlockLoader() : base() { }



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

		lt_dlhandle module = lt_dlopen(block_file_curr.c_str());
		
		if(module != NULL)
		{
			std::cerr << "Loading module file '" << block_file_curr << "' ... ";
			implementation::create_block_func_t create = 0;
			implementation::get_block_name_func_t name = 0;

			reinterpret_cast< void*& >(create) = lt_dlsym(module, "create");
			reinterpret_cast< void*& >(name) = lt_dlsym(module, "name");

			if (create && name)
			{
				const std::string block_id = name();

				if (!(*this)->f_.Register(block_id, create))
				{
					std::cerr << "already exists." << std::endl;
					continue;
				}

				std::cerr << "ok" << " (id=" << block_id << ")" << std::endl;
				++block_count;
				(*this)->available_blocks_.push_back(block_id);
			}
		} else
		{
			std::cerr << lt_dlerror() << " ... ignoring" << std::endl;
		}
	}
	return block_count;
}



Block* BlockLoader::new_block(const std::string& name)
{
 	return (*this)->f_.CreateObject(name);
}



const std::vector< std::string > & BlockLoader::available_blocks() const
{
	implementation const& impl = **this;
	return impl.available_blocks_;
}
