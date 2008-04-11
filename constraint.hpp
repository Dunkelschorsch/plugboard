#ifndef CONSTRAINT_HPP
#define CONSTRAINT_HPP

#ifndef NDEBUG
#include <iostream>
#endif

#include "visibility.hpp"
#include "variable/variable.hpp"
#include "exception/base.hpp"

namespace plugboard
{
	class DSOEXPORT ConstraintTestException : public Exception< std::string >
	{
	public:
		ConstraintTestException(const std::string& unknownId)
			: Exception< std::string >(unknownId)
		{ }

	private:
		const char* do_what() const
		{
			return "constraint check failed";
		}
	};


	class DSOEXPORT ConstraintBase
	{
	public:
		virtual ~ConstraintBase() { }
		inline void throw_exception() const { return do_throw_exception(); }

	private:
		virtual void do_throw_exception() const = 0;
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
		void do_throw_exception() const
		{
			throw ConstraintTestException("blah!");
		}

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

	private:
		result_type do_check(argument_type) const
		{
			return true;
		}

		void do_throw_exception() const
		{
			throw ConstraintTestException("blah!");
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
		void do_throw_exception() const
		{
			throw ConstraintTestException("blah!");
		}

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
		void do_throw_exception() const
		{
			throw ConstraintTestException("blah!");
		}

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
		void do_throw_exception() const
		{
			throw ConstraintTestException("blah!");
		}

		result_type do_check(argument_type arg) const
		{
			return (arg % modulus_) == 0;
		}

		argument_type modulus_;
	};
} // namespace plugboard

#endif // CONSTRAINT_HPP
