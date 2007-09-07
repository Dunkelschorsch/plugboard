#ifndef _BLOCK_LOADER_HPP
#define _BLOCK_LOADER_HPP

class Block;
#include <stdint.h>
#include <string>


class BlockLoader
{
struct BlockLoaderImpl;

public:
	BlockLoader();
	~BlockLoader();

	uint32_t load_dir(const std::string &dir, const bool recursive = false);

	Block* new_block(const std::string &name);

private:
	BlockLoaderImpl *d;
};

#endif //_BLOCK_LOADER_HPP
