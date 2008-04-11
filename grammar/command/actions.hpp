#ifndef GRAMMAR_COMMAND_ACTION_HPP
#define GRAMMAR_COMMAND_ACTION_HPP

#include "grammar/command/policies.hpp"
#include "types/base.hpp"

namespace plugboard
{
	class Variable;

	boost::spirit::parser_id identify_line(const tree_iter_t& i);

	type_t subtree_type(const tree_iter_t& i);

	type_t get_var_type(const std::string& var_name);

	void eval_expression(const tree_iter_t& i, Variable& v);

	template< typename T >
	T make_scalar(const tree_iter_t& i);

	template< typename T >
	void make_range(T start, T end, T inc, Variable& v);

	template< typename T >
	void make_array(tree_iter_t const&  start, tree_iter_t const& end, Variable& v);

	template< typename T >
	T evaluate_scalar_arithmetic(const tree_iter_t& i);

	void eval_assignment(const tree_iter_t& i);

	void eval_add_block(const tree_iter_t& i);

	void eval_connect(const tree_iter_t& i);

	void eval_run(const tree_iter_t& i);

	void eval_command(const tree_iter_t& i);
} // namespace plugboard

#endif // GRAMMAR_COMMAND_ACTION_HPP
