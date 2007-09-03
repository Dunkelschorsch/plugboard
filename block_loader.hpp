#ifndef _BLOCK_LOADER_HPP
#define _BLOCK_LOADER_HPP

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

#include <map>
#include <stdint.h>

#include "block.hpp"

/** The BlockLoader class
 */
class BlockLoader
{
typedef Block* (*create_block_func_t) (void);

typedef const std::string (*get_block_name_func_t) (void);

typedef std::map< std::string, create_block_func_t > block_factory_t;

public:
	BlockLoader() : f_() { };

	uint32_t load_dir(const std::string& dir, const bool recursive = false);

	Block* new_block(const std::string& name);

private:

	block_factory_t f_;
};

#endif //_BLOCK_LOADER_HPP
