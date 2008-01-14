#ifndef _VARIABLE_HPP
#define _VARIABLE_HPP

#include "types.hpp"
#include <boost/any.hpp>



/** The Variable class.
 *
 */
class Variable
{
public:
	Variable();

	Variable(integer_t value);

	Variable(real_t value);

	Variable(const string_t& value);

	Variable(const complex_t& value);

	explicit Variable(std::vector< uint16_t > dimensions);

	void append(const boost::any& value);

	void add_dimension(uint16_t size);

	void set_dimensions(const std::vector< uint16_t >& d);

	const std::vector< uint16_t >& get_dimensions() const;

	type_t get_type() const;

	void set_type(type_t t);

	void save_type_change(type_t t);

	operator bool() const;

	uint64_t size() const;

	friend class iterator;

	class iterator : public std::iterator< std::bidirectional_iterator_tag, boost::any, ptrdiff_t >
	{
	public:
		iterator(std::vector< boost::any >& values_, const std::vector< boost::any >::iterator& i) : r_(&values_), it_(i) {}

		bool operator==(const iterator& x) const;

		bool operator!=(const iterator& x) const;

		std::vector< boost::any >::reference operator*() const;

		iterator& operator++();

		iterator operator++(int);

		iterator& operator--();

		iterator operator--(int);

		iterator insert(const boost::any& x);

		iterator erase();
	
	protected:
		std::vector< boost::any >* r_;
		std::vector< boost::any >::iterator it_;
	};

	iterator begin();

	iterator end();

protected:
	std::vector< uint16_t > dims_;

/** \brief The type of the Variable
 */
	type_t type_;

/** \brief A vector that stores the variable's values.

* detailed description
*/
	std::vector< boost::any > values_;
};

#endif // _VARIABLE_HPP
