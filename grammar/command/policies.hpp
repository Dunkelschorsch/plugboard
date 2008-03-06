#ifndef GRAMMAR_COMMAND_POLICIES_HPP
#define GRAMMAR_COMMAND_POLICIES_HPP

#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/iterator/position_iterator.hpp>

using namespace boost::spirit;


// how the input data gets to the scanner
typedef file_iterator< char > file_iter_t;
typedef position_iterator2< file_iter_t > iterator_t;
// what kind of scanning is done with the input data
typedef tree_match< iterator_t > parse_tree_match_t;
typedef ast_match_policy< iterator_t > match_policy_t;
// activate whitespace skpping
typedef skip_parser_iteration_policy< space_parser > skip_parser_policy_t;

typedef scanner_policies< skip_parser_policy_t, match_policy_t > scanner_policy_t;
typedef scanner< iterator_t, scanner_policy_t > ast_scanner_t;
typedef parse_tree_match_t::const_tree_iterator tree_iter_t;

#endif // GRAMMAR_COMMAND_POLICIES_HPP
