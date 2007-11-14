#ifndef _EXEC_MATRIX
#define _EXEC_MATRIX

#include "exec_stage.hpp"

#include <map>
#include <boost/function.hpp>


class ExecutionMatrix
{
public:
	ExecutionMatrix();
	~ExecutionMatrix();

	void store_block(Block * b, const std::string& name);

	bool block_exists(const std::string& name) const;

	void add_block(const std::string& name);

	void place_block(const std::string& name, const std::string& insert_after);

	bool block_is_placed(const std::string& name) const;

	Block* operator[](const std::string& name) const;

	template< typename IteratorT >
	void swap_stages(IteratorT s1, IteratorT s2);

	const ExecutionStage::store_t& get_stages() const;

	ExecutionStage::store_t& get_stages();

	std::vector< Block * > find_start_blocks();

	void combine_stages();

	void parallelize();

	void init();

	void exec();

protected:
	void add_block(Block * b);
	void add_block(Block * b, const std::string& insert_after);
	void add_block(Block * b, uint32_t insert_where);

	void add_stage(ExecutionStage s);
	
	typedef std::map< std::string, Block * > block_map_t;

	ExecutionStage::store_t stages_;
	block_map_t blocks_;
};



namespace std
{
	template< typename C, typename E >
	basic_ostream< C, E >& operator<<(basic_ostream< C, E >& out, const ExecutionMatrix& what)
	{
#ifndef NDEBUG
		out << std::endl << "-------------" << std::endl;
#endif
		for(uint32_t j=0; j<what.get_stages().size(); ++j)
		{
			out << "Stage: " << j << std::endl;
			out << what.get_stages()[j];
		}
#ifndef NDEBUG
		out << "-------------" << std::endl << std::endl;
#endif
		return out;
	}
}

#endif // _EXEC_MATRIX
