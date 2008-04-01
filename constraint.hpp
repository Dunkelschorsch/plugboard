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

	explicit ValueConstraint(bool negative=false) : negative_(negative) { }
	virtual ~ValueConstraint() { }

	inline result_type check(argument_type arg) const { return do_check(arg); }
	bool is_negative() const { return negative_; }

	bool negative_;
private:
	virtual result_type do_check(argument_type arg) const = 0;
};



class DSOEXPORT VariableConstraint
{
public:
	typedef bool result_type;

	explicit VariableConstraint(bool negative=false) : negative_(negative) { }
	virtual ~VariableConstraint() { }

	inline result_type check(const Variable& var) const { return do_check(var); }
	bool is_negative() const { return negative_; }

private:
	virtual result_type do_check(const Variable& var) const = 0;

	bool negative_;
};



class SizeConstraint : public ConstraintBase, public VariableConstraint
{
typedef VariableConstraint::result_type result_type;

public:
	explicit SizeConstraint(uint64_t numel, bool neg=false) :
		VariableConstraint(neg),
		numel_(numel)
	{ }
	
private:
	result_type do_check(const Variable& var) const
	{
		return var.size() == numel_;
	}

	uint64_t numel_;
};



template< typename T >
class NullConstraint : public ConstraintBase, public ValueConstraint< T >
{
typedef typename ValueConstraint< T >::argument_type argument_type;
typedef typename ValueConstraint< T >::result_type result_type;

	result_type do_check(argument_type) const
	{
		return true;
	}
};



template< typename T >
class LessThanConstraint : public ConstraintBase, public ValueConstraint< T >
{
typedef typename ValueConstraint< T >::argument_type argument_type;
typedef typename ValueConstraint< T >::result_type result_type;

public:
	explicit LessThanConstraint(argument_type compare, bool neg=false) :
		ValueConstraint< T >(neg),
		compare_(compare)
	{ }

private:
	result_type do_check(argument_type arg) const
	{
		return arg < compare_ ? true : false;
	}

	argument_type compare_;
};



template< typename T >
class GreaterThanConstraint : public ConstraintBase, public ValueConstraint< T >
{
typedef typename ValueConstraint< T >::argument_type argument_type;
typedef typename ValueConstraint< T >::result_type result_type;

public:
	explicit GreaterThanConstraint(argument_type compare, bool neg=false) :
		ValueConstraint< T >(neg),
		compare_(compare)
	{ }

private:
	result_type do_check(argument_type arg) const
	{
		return arg > compare_ ? true : false;
	}

	argument_type compare_;
};



template< typename T >
class ModuloConstraint : public ConstraintBase, public ValueConstraint< T >
{
typedef typename ValueConstraint< T >::argument_type argument_type;
typedef typename ValueConstraint< T >::result_type result_type;

public:
	explicit ModuloConstraint(argument_type modulus, bool neg=false) :
		ValueConstraint< T >(neg),
		modulus_(modulus)
	{ }

private:
	result_type do_check(argument_type arg) const
	{
		return (arg % modulus_) == 0;
	}

	argument_type modulus_;
};

#endif // CONSTRAINT_HPP
