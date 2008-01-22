#ifndef BLOCK_LOADER_HPP
#define BLOCK_LOADER_HPP

class Block;
#include "types/base.hpp"
#include <vector>
#include <string>
#include <boost/pool/detail/singleton.hpp>



class BlockLoader
{
struct BlockLoaderImpl;

typedef boost::details::pool::singleton_default< BlockLoader > singleton;
friend class boost::details::pool::singleton_default< BlockLoader >;

public:
	uint32_t load_dir(const std::string& dir, const bool recursive = false);

	const std::vector< std::string > & available_blocks() const;

	Block* new_block(const std::string &name) const;

	static BlockLoader& instance()
	{
		BlockLoader & bl = singleton::instance();
		return bl;
	}

protected:
	BlockLoader();
	~BlockLoader();

private:
	BlockLoaderImpl *d;
};

#endif //BLOCK_LOADER_HPP
