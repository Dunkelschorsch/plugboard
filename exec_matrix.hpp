#ifndef EXEC_MATRIX_HPP
#define EXEC_MATRIX_HPP

#include "exec_stage.hpp"
#include "pimpl/pimpl.h"

#include <iosfwd>
#include <map>
#include <boost/function.hpp>


class ExecutionMatrix : public pimpl< ExecutionMatrix >::pointer_semantics
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

	const ExecutionStage::store_t& get_stages() const;

	const std::vector< Block * > find_start_blocks() const;

	void combine_stages();

	void parallelize();

	void init();

	void exec();

	void print(std::ostream&) const;
};



std::ostream& operator<<(std::ostream&, const ExecutionMatrix&);

#endif // EXEC_MATRIX_HPP
