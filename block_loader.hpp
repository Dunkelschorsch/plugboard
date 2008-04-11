#ifndef BLOCK_LOADER_HPP
#define BLOCK_LOADER_HPP

#include "types/base.hpp"
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
