#include <boost/function.hpp>
#include <iostream>
#include <map>

#include "block/block.hpp"
#include "variable/variable.hpp"
#include "variable/variable_iterator.hpp"
#include "exceptions.hpp"
#include "constraint.hpp"


struct Block::BlockImpl
{
	BlockImpl() :
		param_curr_(0),
		parameter_factory_(),
		params_(),
		configured_(true),
		name_sys_(),
		name_(),
		description_()
	{
		register_parameter_types();
	}

	~BlockImpl();

	typedef boost::function< void(Variable&, const Parameter*) > parameter_factory_func_t;
	typedef std::map< type_t, parameter_factory_func_t > parameter_factory_t;

	uint16_t param_curr_;
	parameter_factory_t parameter_factory_;
	std::vector< const Parameter* > params_;
	bool configured_;

	std::string name_sys_;
	std::string name_;
	std::string description_;


	template < typename T >
	void copy_parameter(Variable&, const Parameter*);

	void register_parameter_types();
};



Block::BlockImpl::~BlockImpl()
{
	std::vector< const Parameter* >::iterator p_it;
	for(p_it = params_.begin(); p_it != params_.end(); ++p_it)
	{
		delete *p_it;
	}
}



Block::Block() :
	d(new Block::BlockImpl) { }



Block::~Block()
{
	delete d;
}



void Block::initialize() { }



void Block::advance() { }



const std::string& Block::get_name() const
{
	return d->name_;
}



void Block::set_name( const std::string & name )
{
	d->name_ = name;
}



const std::string& Block::get_name_sys() const
{
	return d->name_sys_;
}



void Block::set_name_sys(const std::string & name_sys)
{
	d->name_sys_ = name_sys;
}



const std::string& Block::get_description() const
{
	return d->description_;
}



void Block::set_description(const std::string& description)
{
	d->description_ = description;
}



bool Block::is_configured() const
{
	return d->configured_;
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
		const ValueConstraint< TargetT >* c = dynamic_cast< const ValueConstraint< TargetT >* >(cb);
		bool passed = c->check(e_);
		if(c->is_negative())
			passed = not passed;

		if(not passed)
		{
#ifndef NDEBUG
			std::cout << "aww!" << std::endl;
#endif
			// TODO come up with proper exceptions here
			throw std::exception();
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
	CopyAction(const Parameter * const param) : param_(param) { }

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

	const Parameter * const param_;
};



template< typename VariableElementT >
void Block::BlockImpl::copy_parameter(Variable& var, const Parameter * const param)
{
	variable_iterator< VariableElementT > begin = variable_iterator< VariableElementT >(var);
	variable_iterator< VariableElementT > end = begin.make_end();

#ifndef NDEBUG
	std::cout << "Parameter name: " << param->get_description() << std::endl;
	std::cout << "no. of constraints: " << param->get_constraints().size() << std::endl;
#endif
	std::for_each(begin, end, CopyAction< VariableElementT >(param));
	param_curr_++;
}



void Block::add_parameter(const Parameter* p)
{
	d->configured_ = false;
	d->params_.push_back(p);
}



bool Block::set_parameter(const Variable& p)
{
	if (is_configured())
	{
		std::cout << "Block is already configured." << std::endl;
		return false;
	}

	// check if type is safely convertible
	if (d->params_[d->param_curr_]->is_convertible_to(p))
	{
		// we do not want to typecast the original variable
		Variable var_tmp(p);

		if(not d->params_[d->param_curr_]->is_of_same_type_as(p))
		{
#ifndef NDEBUG
			std::cout << "changing type of variable." << std::endl;
#endif
			var_tmp.save_type_change(d->params_[d->param_curr_]->get_type());
		}

		Block::BlockImpl::parameter_factory_func_t
			fill_block_parameter_with_values_from_variable = d->parameter_factory_[var_tmp.get_type()];

		fill_block_parameter_with_values_from_variable(var_tmp, d->params_[d->param_curr_]);

		// if this was the last parameter the block is completely configured
		if (d->params_.size() == d->param_curr_)
		{
			d->configured_ = true;
		}
	} else
	{
		throw IncompatibleTypesException(get_name_sys());
	}
	return true;
}



const std::string& Block::get_parameter_description() const
{
	return d->params_[d->param_curr_]->get_description();
}



type_t Block::get_parameter_type() const
{
	return d->params_[d->param_curr_]->get_type();
}



void Block::BlockImpl::register_parameter_types()
{
/* this macro creates mapping between types and parameters */
#define BOOST_PP_DEF(z, I, _) \
	parameter_factory_.insert(std::make_pair(TYPE_VALUE(I),	\
	boost::bind(&BlockImpl::copy_parameter< CPP_TYPE(I) >, this, _1, _2)));

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF
}



const std::vector< const Parameter* >& Block::get_params() const
{
	return d->params_;
}
