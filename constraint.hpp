#ifndef CONSTRAINT_HPP
#define CONSTRAINT_HPP

#ifndef NDEBUG
#include <iostream>
#endif

#include "visibility.hpp"
#include "variable/variable.hpp"


struct DSOEXPORT ConstraintBase
{
	virtual ~ConstraintBase() { }
};



template< class T >
class DSOEXPORT ValueConstraint
{
public:
	typedef const T argument_type;
	typedef bool result_type;

	ValueConstraint(bool negative=false) : negative_(negative) { }
	virtual ~ValueConstraint() { }

	virtual result_type check(argument_type arg) const = 0;
	bool is_negative() const { return negative_; }

	bool negative_;
};



class DSOEXPORT VariableConstraint
{
public:
	typedef bool result_type;

	VariableConstraint(bool negative=false) : negative_(negative) { }
	virtual ~VariableConstraint() { }

	virtual result_type check(const Variable& var) const = 0;
	bool is_negative() const { return negative_; }

	bool negative_;
};



class SizeConstraint : public ConstraintBase, public VariableConstraint
{
typedef VariableConstraint::result_type result_type;

public:
	SizeConstraint(uint64_t numel, bool neg=false) :
		VariableConstraint(neg),
		numel_(numel)
	{ }
	

	result_type check(const Variable& var) const
	{
		return var.size() == numel_;
	}

private:
	uint64_t numel_;
};



template< typename T >
class NullConstraint : public ConstraintBase, public ValueConstraint< T >
{
typedef typename ValueConstraint< T >::argument_type argument_type;
typedef typename ValueConstraint< T >::result_type result_type;

public:
	result_type check(argument_type) const
	{
		return true;
	}

	~NullConstraint() { }
};



template< typename T >
class LessThanConstraint : public ConstraintBase, public ValueConstraint< T >
{
typedef typename ValueConstraint< T >::argument_type argument_type;
typedef typename ValueConstraint< T >::result_type result_type;

public:
	LessThanConstraint(argument_type compare, bool neg=false) :
		ValueConstraint< T >(neg),
		compare_(compare)
	{ }

	result_type check(argument_type arg) const
	{
		return arg < compare_ ? true : false;
	}

private:
	argument_type compare_;
};



template< typename T >
class GreaterThanConstraint : public ConstraintBase, public ValueConstraint< T >
{
typedef typename ValueConstraint< T >::argument_type argument_type;
typedef typename ValueConstraint< T >::result_type result_type;

public:
	GreaterThanConstraint(argument_type compare, bool neg=false) :
		ValueConstraint< T >(neg),
		compare_(compare)
	{ }

	result_type check(argument_type arg) const
	{
		return arg > compare_ ? true : false;
	}

private:
	argument_type compare_;
};


#endif // CONSTRAINT_HPP
