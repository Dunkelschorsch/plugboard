#ifndef _EXEC_STAGE
#define _EXEC_STAGE

#include <deque>
#include <iosfwd>

#include "block/block.hpp"

namespace boost { class thread; }



class ExecutionStage
{
friend class ExecutionMatrix;
public:
	typedef std::deque< ExecutionStage > store_t;
	typedef Block::store_t path_t;
	typedef std::deque< path_t > stage_t;

	ExecutionStage();

	explicit ExecutionStage(Block *b, bool threading_enabled=false);

	void add_block(Block *b);
	void add_path(const path_t& p);

	const stage_t& get_paths() const;
	stage_t& get_paths();

	Block* operator[](const std::string& name) const;

protected:
	stage_t paths_;
	bool threading_enabled_;
	std::vector< boost::thread* > thread_group_;

	bool block_is_placed(const std::string& name) const;

	void exec();
	void exec_path(const path_t& p);
};



namespace std
{
	template< typename C, typename E >
	basic_ostream< C, E > & operator<<(basic_ostream< C, E > & out, const ExecutionStage& what)
	{
		for(uint32_t j=0; j<what.get_paths().size(); ++j)
		{
			out << "  Path: " << j << std::endl;
			for(uint32_t k=0; k<what.get_paths()[j].size(); ++k)
			{
				out << "   " << what.get_paths()[j][k]->get_name_sys();
			}
			out << std::endl;
		}
		return out;
	}
}

#endif // _EXEC_STAGE
