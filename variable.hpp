#ifndef _VARIABLE_HPP
#define _VARIABLE_HPP

#ifndef NDEBUG
#include <iostream>
#endif

#include "types/base.hpp"
#include "types/typeinfo.hpp"

#include <algorithm>
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
	Variable(const Variable& other);
	~Variable();

	Variable& operator=(const Variable& other);
	operator bool() const;

	template< typename ElementT >
	void push_back(const ElementT);

	void add_dimension(uint16_t size);

	void set_dimensions(const std::vector< uint16_t >& d);

	const std::vector< uint16_t >& get_dimensions() const;

	type_t get_type() const;

	void set_type(type_t t);

	bool save_type_change(type_t t);

	// just for the iterator. use at your own risk!
	// no. wait. don't use it at all...
	void* data() const;

	size_t size() const;

protected:

	template< typename oldT, typename newT >
	void cast();

	void swap(Variable & other);

	std::vector< uint16_t > dims_;

	uint64_t numel_;
	uint64_t size_;
	void *data_;
	type_t type_;
};




template< typename ElementT >
void Variable::push_back(const ElementT e)
{
	if(type_ == empty)
		type_ = typeinfo< ElementT >::value;
		
	data_ = realloc(data_, typeinfo< ElementT >::size * (numel_+1));
	static_cast< ElementT* >(data_)[numel_++] = static_cast< const ElementT >(e);

	size_ += typeinfo< ElementT >::size;
}



template< typename oldT, typename newT >
void Variable::cast()
{
	void* new_data = malloc(typeinfo< newT >::size * this->numel_);
	for(size_t i=0; i<this->numel_; ++i)
	{
		static_cast< newT* >(new_data)[i] =
			static_cast< newT >(static_cast< oldT* >(this->data_)[i]);
	}
	free(data_);
	data_ = new_data;

	size_ = numel_ * typeinfo< newT >::size;
#ifndef NDEBUG
	std::cout << "new variable size (bytes): " << size_ << std::endl;
#endif
}

#endif // _VARIABLE_HPP
