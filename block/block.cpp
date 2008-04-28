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

#include <iostream>
#include <map>
#include <tr1/functional>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/casts.hpp>
#include <boost/lambda/construct.hpp>

#include "block/block.hpp"
#include "constraint.hpp"
#include "variable/variable.hpp"
#include "variable/variable_iterator.hpp"
#include "exception/input.hpp"

using namespace plugboard;


template< >
struct pimpl< Block >::implementation
{
	implementation() :
		param_curr_(0),
		parameter_factory_(),
		params_(),
		configured_(true),
		initialized_(false),
		name_sys_(),
		name_(),
		description_() 
	{
		register_parameter_types();
	}

	~implementation();

	typedef std::tr1::function< void(const Variable&, Parameter * const) > parameter_factory_func_t;
	typedef std::map< type_t, parameter_factory_func_t > parameter_factory_t;

	uint16_t param_curr_;
	parameter_factory_t parameter_factory_;
	std::vector< Parameter* > params_;
	bool configured_;
	bool initialized_;

	std::string name_sys_;
	std::string name_;
	std::string description_;

	template< typename T >
	void copy_parameter(const Variable&, Parameter * const);

	void register_parameter_types();
};


pimpl< Block >::implementation::~implementation()
{
	std::for_each
	(
		params_.begin(),
		params_.end(),
		boost::lambda::bind(boost::lambda::delete_ptr(), boost::lambda::_1)
	);
}


void pimpl< Block >::implementation::register_parameter_types()
{
using std::tr1::placeholders::_1;
using std::tr1::placeholders::_2;
/* this macro creates mapping between types and parameters */
#define BOOST_PP_DEF(z, I, _) \
	parameter_factory_.insert(std::make_pair(TYPE_VALUE(I),	\
	std::tr1::bind(&implementation::copy_parameter< CPP_TYPE(I) >, this, _1, _2)));

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF
}


Block::Block() : base() { }
Block::~Block() { }

// virtual members with empty default implementation
void Block::configure_parameters() { }

void Block::initialize() { }

void Block::advance() { }

void Block::finalize() { }

// non-virtual interface functions
void Block::call_process()
{
	process();
}

void Block::call_initialize()
{
	initialize();
}

void Block::call_advance()
{
	advance();
}

void Block::call_finalize()
{
	finalize();
}

void Block::call_configure_parameters()
{
	configure_parameters();
}


const std::string& Block::get_name() const
{
	return (*this)->name_;
}


void Block::set_name( const std::string & name )
{
	(*this)->name_ = name;
}


const std::string& Block::get_name_sys() const
{
	return (*this)->name_sys_;
}


void Block::set_name_sys(const std::string & name_sys)
{
	(*this)->name_sys_ = name_sys;
}


const std::string& Block::get_description() const
{
	return (*this)->description_;
}


void Block::set_description(const std::string& description)
{
	(*this)->description_ = description;
}


bool Block::is_configured() const
{
	return (*this)->configured_;
}


bool Block::is_initialized() const
{
	return (*this)->initialized_;
}


void Block::set_initialized()
{
	(*this)->initialized_ = true;
}


template< typename TargetT >
struct CheckConstraintAction
{
	CheckConstraintAction(const TargetT& e) : e_(e) { }

	void operator()(const ConstraintBase * const cb) const
	{
#ifndef NDEBUG
		std::cout << "checking constraint... ";
#endif
		const ValueConstraint< TargetT >* c
			= dynamic_cast< const ValueConstraint< TargetT >* >(cb);

		bool passed = c->check(e_);
		if(c->is_negative())
			passed = not passed;

		if(not passed)
		{
#ifndef NDEBUG
			std::cout << "aww!" << std::endl;
#endif
			// TODO come up with proper exceptions here
			cb->throw_exception();
		}
#ifndef NDEBUG
		else
		{
			std::cout << "yay!" << std::endl;
		}
#endif
	}

	const TargetT& e_;
};


template< typename TargetT >
struct CopyAction
{
	CopyAction(Parameter * const param) : param_(param) { }

	void operator()(const TargetT& e) const
	{
		std::for_each
		(
			param_->get_constraints().begin(),
			param_->get_constraints().end(),
			CheckConstraintAction< TargetT >(e)
		);

		static_cast< std::vector< TargetT >* >(param_->get_data())->push_back(e);
	}

	 Parameter * const param_;
};


struct CheckVariableConstraintAction
{
	CheckVariableConstraintAction(const Variable& var) : var_(var) { }

	void operator()(const ConstraintBase * const cb) const
	{
		const VariableConstraint* c =
			dynamic_cast< const VariableConstraint* >(cb);
		if(c)
		{
			bool passed = c->check(var_);
			if(c->is_negative())
				passed = not passed;

			if(not passed)
			{
				cb->throw_exception();
			}
		}
	}

	const Variable& var_;
};


template< typename VariableElementT >
void pimpl< Block >::implementation::copy_parameter(const Variable& var, Parameter * const param)
{
	typedef variable_iterator< VariableElementT const, Variable const > variable_const_iterator;

#ifndef NDEBUG
	std::cout << "Parameter name: " << param->get_description() << std::endl;
	std::cout << "no. of constraints: " << param->get_constraints().size() << std::endl;
#endif
	variable_const_iterator begin(var);
	variable_const_iterator end = begin.make_end();

	// check all constraints regarding parameters of the variable
	std::for_each
	(
		param->get_constraints().begin(),
		param->get_constraints().end(),
		CheckVariableConstraintAction(var)
	);

	using namespace boost::lambda;

	// remove variable constraints
	std::vector< const ConstraintBase* >::iterator var_constraints_begin =
		std::remove_if
		(
			param->get_constraints().begin(),
			param->get_constraints().end(),
			ll_dynamic_cast< const VariableConstraint* >(boost::lambda::_1)
		);

	std::for_each
	(
		var_constraints_begin,
		param->get_constraints().end(),
		bind(delete_ptr(), boost::lambda::_1)
	);

	param->get_constraints().erase
	(
		var_constraints_begin,
		param->get_constraints().end()
	);

	// check all constraints regarding values of the variable
	std::for_each
	(
		begin,
		end,
		CopyAction< VariableElementT >(param)
	);

	param_curr_++;
}


template< class T >
ParameterTypedProxy< T > * Block::add_parameter(std::vector< T > *v, const std::string & description)
{
	Parameter *p = new Parameter(v, plugboard::typeinfo< T >::value, description);
	(*this)->configured_ = false;
	(*this)->params_.push_back(p);

	p->proxy = malloc(sizeof(ParameterTypedProxy< T >));
	p->proxy = new (p->proxy) ParameterTypedProxy< T >((*this)->params_.back());

	return static_cast< ParameterTypedProxy< T >* >(p->proxy);
}


// instantiate for all possible types. we cannot define this function in the .hpp file
// beause we need access to the private implementation which is only forward declared there
#define BOOST_PP_DEF(z, I, _) \
	template ParameterTypedProxy<CPP_TYPE(I)>* Block::add_parameter(std::vector< CPP_TYPE(I) >* v, const std::string& description); \
	
	BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF


bool Block::set_parameter(const Variable& p)
{
	implementation& impl = **this;

	if (is_configured())
	{
		std::cout << "Block is already configured." << std::endl;
		return false;
	}

	// check if type is safely convertible
	if (impl.params_[impl.param_curr_]->is_convertible_to(p))
	{
		// we do not want to typecast the original variable
		Variable var_tmp(p);

		if(not impl.params_[impl.param_curr_]->is_of_same_type_as(p))
		{
#ifndef NDEBUG
			std::cout << "changing type of variable." << std::endl;
#endif
			var_tmp.save_type_change(impl.params_[impl.param_curr_]->get_type());
		}

		implementation::parameter_factory_func_t
			fill_block_parameter_with_values_from_variable = impl.parameter_factory_[var_tmp.get_type()];

		fill_block_parameter_with_values_from_variable(var_tmp, impl.params_[impl.param_curr_]);

		// if this was the last parameter the block is completely configured
		if (impl.params_.size() == impl.param_curr_)
		{
			impl.configured_ = true;
		}
	} else
	{
		throw IncompatibleTypesException(get_name_sys());
	}
	return true;
}


const std::string& Block::get_parameter_description() const
{
	return (*this)->params_[(*this)->param_curr_]->get_description();
}


type_t Block::get_parameter_type() const
{
	return (*this)->params_[(*this)->param_curr_]->get_type();
}


const std::vector< Parameter* >& Block::get_params() const
{
	return (*this)->params_;
}
