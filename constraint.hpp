/*!
 * \file
 * \brief
 * \author Armin Schmidt
 *
 * ----------------------------------------------------------------------------
 *
 * PlugBoard - A versatile communication simulation framework
 * Copyright (C) 2007-2008  Armin Schmidt
 *
 * This file is part of PlugBoard.
 *
 * PlugBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PlugBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PlugBoard.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ----------------------------------------------------------------------------
 */

#ifndef CONSTRAINT_HPP
#define CONSTRAINT_HPP

#include "visibility.hpp"
#include "variable/variable.hpp"
#include "exception/base.hpp"
#include "boost/shared_ptr.hpp"

namespace plugboard
{
	struct Constraint
	{
	      static const bool reverse = true;
	};

	class ConstraintBase;
	typedef boost::shared_ptr< ConstraintBase > constraint_ptr;

	class ConstraintTestException : public Exception< std::string >
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


	class ConstraintBase
	{
	public:
		virtual ~ConstraintBase() { }
		void throw_exception() const { return do_throw_exception(); }

	private:
		virtual void do_throw_exception() const = 0;
	};



	template< class T >
	class ValueConstraint
	{
	public:
		typedef const T argument_type;
		typedef bool result_type;

		explicit ValueConstraint(bool negative=false) : negative_(negative) { }
		virtual ~ValueConstraint() { }

		result_type check(argument_type arg) const { return do_check(arg); }
		bool is_negative() const { return negative_; }

		bool negative_;
	private:
		virtual result_type do_check(argument_type arg) const = 0;
	};



	class VariableConstraint
	{
	public:
		typedef bool result_type;

		explicit VariableConstraint(bool negative=false) : negative_(negative) { }
		virtual ~VariableConstraint() { }

		result_type check(const Variable& var) const { return do_check(var); }
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
			throw ConstraintTestException("SizeConstraint");
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
			throw ConstraintTestException("NullConstraint");
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
			throw ConstraintTestException("LessThanConstraint");
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
			throw ConstraintTestException("GreaterThanConstraint");
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
			throw ConstraintTestException("ModuloConstraint");
		}

		result_type do_check(argument_type arg) const
		{
			return (arg % modulus_) == 0;
		}

		argument_type modulus_;
	};
} // namespace plugboard

#endif // CONSTRAINT_HPP
