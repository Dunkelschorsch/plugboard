#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#ifndef NDEBUG
#include <iostream>
#endif

#include <vector>
#include "visibility.hpp"
#include "types/base.hpp"
#include "types/typeinfo.hpp"



class DSOEXPORT Variable
{
public:
	Variable();
	Variable(int32_t value);
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

	void prealloc(size_t);
protected:

	template< typename oldT, typename newT >
	void cast() DSOLOCAL;

	void swap(Variable & other) DSOLOCAL;

	std::vector< uint16_t > dims_;

	uint64_t numel_;
	uint64_t allocated_;
	uint64_t size_;
	void *data_;
	type_t type_;
};



template< typename ElementT >
void Variable::push_back(const ElementT e)
{
	if(type_ == empty)
		type_ = typeinfo< ElementT >::value;

	if(allocated_ == numel_)
	{
#ifndef NDEBUG
		std::cout << "reallocating..." << std::endl;
#endif
		data_ = realloc(data_, typeinfo< ElementT >::size * (numel_+1));
		allocated_ = numel_+1;
	}

	static_cast< ElementT* >(data_)[numel_++] = static_cast< const ElementT >(e);

	size_ += typeinfo< ElementT >::size;
}



#endif // VARIABLE_HPP
