#ifndef _VARIABLE_ITERATOR_HPP
#define _VARIABLE_ITERATOR_HPP

#include <cassert>
#include <boost/iterator/iterator_facade.hpp>


template< typename T >
class variable_iterator : public boost::iterator_facade< variable_iterator< T >, T, boost::forward_traversal_tag >
{
public:
	variable_iterator() : var_(0), value_(0), pos_(0) { }

	explicit variable_iterator(Variable& v) : var_(v),  value_(0), pos_(0)
	{
		increment();
	}

	explicit variable_iterator(Variable& v, size_t pos) : var_(v), value_(0), pos_(1+pos) { }

	variable_iterator< T > make_end()
	{
		variable_iterator< T > tmp = variable_iterator< T >(var_);
		tmp.pos_ = var_.size()+1;
		return tmp;
	}

private:
	friend class boost::iterator_core_access;

	void increment()
	{
		assert(pos_ <= var_.size());
		value_ = &static_cast< T* >(var_.data())[pos_++];
	}

	bool equal(const variable_iterator& other) const
	{
		return this->pos_ == other.pos_;
	}

	T& dereference() const
	{
		return *value_;
	}

	Variable& var_;
	T* value_;
	size_t pos_;
};

#endif // _VARIABLE_ITERATOR_HPP
