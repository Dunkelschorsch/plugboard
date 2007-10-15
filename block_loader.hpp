#ifndef _BLOCK_LOADER_HPP
#define _BLOCK_LOADER_HPP

class Block;
#include "types.hpp"
#include <string>


class BlockLoader
{
struct BlockLoaderImpl;

public:
	BlockLoader();
	~BlockLoader();

	uint32_t load_dir(const std::string &dir, const bool recursive = false);

	const std::vector< std::string > & available_blocks() const;

	Block* new_block(const std::string &name) const;

private:
	BlockLoaderImpl *d;
};

#endif //_BLOCK_LOADER_HPP
