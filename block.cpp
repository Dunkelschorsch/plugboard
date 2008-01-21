#include "block.hpp"
#include "variable/variable.hpp"
#include "variable/variable_iterator.hpp"
#include "exceptions.hpp"

#include <boost/bind.hpp>
#include <iostream>



Block::Block() :
	parameter_factory_(),
	params_(),
	param_curr_(0),
	configured_(true),
	name_(),
	description_(),
	ports_in_(),
	ports_out_(),
	connected_blocks_(),
	name_sys_(),
	num_output_ports_(0),
	num_input_ports_(0)
{
	register_parameter_types();
}



Block::~Block()
{

}



void Block::initialize()
{

}



void Block::advance()
{

}



// const std::string& Block::get_name() const
// {
// 	return name_;
// }
// 
// 
// 
// const std::string& Block::get_name_sys() const
// {
// 	return name_sys_;
// }
// 
// 
// 
// void Block::set_name_sys(const std::string & name_sys)
// {
// 	name_sys_ = name_sys;
// }
// 
// 
// 
// const std::string& Block::get_description() const
// {
// 	return description_;
// }
// 
// 
// 
// void Block::set_description(const std::string& description)
// {
// 	description_ = description;
// }
// 
// 
// 
// bool Block::is_configured() const
// {
// 	return configured_;
// }



template< typename TargetT >
struct CopyAction
{
	CopyAction(void * const out) : out_(out) { }

	template< typename ElementT >
	void operator()(ElementT e) const
	{
		static_cast< std::vector< TargetT >* >(out_)->push_back(e);
	}

	void * const out_;
};



template< typename T >
void Block::copy_parameter(void *out, Variable& p)
{
	variable_iterator< T > begin = variable_iterator< T >(p);
	variable_iterator< T > end = begin.make_end();

	std::for_each
	(
		begin,
		end,
		CopyAction< T >(out)
	);
	param_curr_++;
}



void Block::add_parameter(void* var, type_t t, std::string description)
{
	configured_ = false;
	params_.push_back(Parameter(var, t, description));
}



bool Block::set_parameter(const Variable& p)
{
	if (is_configured())
	{
		std::cout << "Block is already configured." << std::endl;
		return false;
	}

	// check if type is safely convertible
	if (params_[param_curr_].is_convertible_to(p))
	{
		// we do not want to typecast the original variable
		Variable var_tmp(p);

		if(not params_[param_curr_].is_of_same_type_as(p))
		{
#ifndef NDEBUG
			std::cout << "changing type of variable." << std::endl;
#endif
			var_tmp.save_type_change(params_[param_curr_].get_type());
		}

		boost::function< void(void*, Variable&) >
			fill_block_parameter_with_values_from_variable = parameter_factory_[var_tmp.get_type()];

		fill_block_parameter_with_values_from_variable(params_[param_curr_].get_data(), var_tmp);

		// if this was the last parameter the block is completely configured
		if (params_.size() == param_curr_)
		{
			configured_ = true;
		}
	} else
	{
		throw IncompatibleTypesException(get_name_sys());
	}
	return true;
}



// const std::string& Block::get_parameter_description() const
// {
// 	return params_[param_curr_].get_description();
// }
// 
// 
// 
// type_t Block::get_parameter_type() const
// {
// 	return params_[param_curr_].get_type();
// }



void Block::register_parameter_types()
{
/* this macro creates mapping between types and parameters */
#define BOOST_PP_DEF(z, I, _) \
	parameter_factory_.insert(std::make_pair(TYPE_VALUE(I),	\
	boost::bind(&Block::copy_parameter< CPP_TYPE(I) >, this, _1, _2)));

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

#undef BOOST_PP_DEF
}



// bool Block::setup_output_ports()
// {
// 	return false;
// }
// 
// 
// 
// bool Block::setup_input_ports()
// {
// 	return false;
// }
// 
// 
// 
// uint16_t Block::get_num_input_ports() const
// {
// 	return num_input_ports_;
// }
// 
// 
// 
// uint16_t Block::get_num_output_ports() const
// {
// 	return num_output_ports_;
// }
// 
// 
// 
// InPort::store_t & Block::get_inport_list()
// {
// 	return ports_in_;
// }
// 
// 
// 
// OutPort::store_t & Block::get_outport_list()
// {
// 	return ports_out_;
// }
// 
// 
// 
// const std::vector< Parameter >& Block::get_params() const
// {
// 	return params_;
// }
// 
// 
// 
// void Block::add_connection(const std::string & name)
// {
// 	connected_blocks_.insert(name);
// }
// 
// 
// 
// const std::set< std::string >& Block::get_connections() const
// {
// 	return connected_blocks_;
// }
