#include "block.hpp"
#include "port.hpp"
#include "variable.hpp"

#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <iostream>

Block::Block()
{
	configured_ = false;
	param_curr_ = 0;
}

std::string Block::get_name() const
{
	return name_;
}

std::string Block::get_description() const
{
	return description_;
}

void Block::set_description(const std::string& description)
{
	description_ = description;
}


bool Block::is_configured() const
{
	return configured_;
}


bool Block::set_parameter(const Variable& pp)
{
	int_vec_t     *tmp_i_vec;
	real_vec_t    *tmp_d_vec;
	string_vec_t  *tmp_s_vec;
	complex_vec_t *tmp_c_vec;


	/* FIXME This is evil! Implement a const_iterator instead! */
	Variable p = const_cast< Variable& >(pp);

	if (is_configured())
	{
		std::cout << "Block is already configured." << std::endl;
		return false;
	}

	if (params_[param_curr_].second == p.type())
	{
		switch (p.type())
		{
		case integer:
			tmp_i_vec = reinterpret_cast< std::vector< integer_t >* >(params_[param_curr_].first);
			for(Variable::iterator it = p.begin(); it != p.end(); it++)
			{
				tmp_i_vec->push_back(boost::any_cast< integer_t >(*it));
			}
			param_curr_++;
			break;

		case real:
			tmp_d_vec = reinterpret_cast< std::vector< real_t >* >(params_[param_curr_].first);
			for(Variable::iterator it = p.begin(); it != p.end(); it++)
			{
				tmp_d_vec->push_back(boost::any_cast< real_t >(*it));
			}
			param_curr_++;
			break;

		case string:
			tmp_s_vec = reinterpret_cast< std::vector< string_t >* >(params_[param_curr_].first);
			for(Variable::iterator it = p.begin(); it != p.end(); it++)
			{
				tmp_s_vec->push_back(boost::any_cast< string_t >(*it));
			}
			param_curr_++;
			break;

		case complex:
			tmp_c_vec = reinterpret_cast< std::vector< complex_t >* >(params_[param_curr_].first);
			for(Variable::iterator it = p.begin(); it != p.end(); it++)
			{
				tmp_c_vec->push_back(boost::any_cast< complex_t >(*it));
			}
			param_curr_++;
			break;

		default:
			return false;
		}

		if (params_.size() == param_curr_)
		{
			configured_ = true;
		}
	} else
	{
		return false;
	}
	return true;
}


OutPort* Block::add_port(const OutPort& p)
{
	OutPort::store_t::iterator it =
		std::find_if(ports_out_.begin(), ports_out_.end(), boost::bind(&BasePort::get_name, _1) == p.get_name());

	if (it != ports_out_.end())
	{
		throw duplicate_port_name_error(get_name() + "::" + p.get_name());
	}
	ports_out_.push_back(p);
	
	return &(*(ports_out_.end()-1));
}


InPort* Block::add_port(const InPort& p)
{
	InPort::store_t::iterator it =
		std::find_if(ports_in_.begin(), ports_in_.end(), boost::bind(&BasePort::get_name, _1) == p.get_name());

	if (it != ports_in_.end())
	{
		throw duplicate_port_name_error(get_name() + "::" + p.get_name());
	}
	ports_in_.push_back(p);

	return &(*(ports_in_.end()-1));
}


Block::~Block()
{

}


void Block::set_name_sys(const std::string & name_sys)
{
	name_sys_ = name_sys;
}


std::string Block::get_name_sys()
{
	return name_sys_;
}


/*!
    \fn Block::add_parameter()
 */
void Block::add_parameter(void* var, type_t t)
{
	params_.push_back(std::make_pair(var, t));
}

bool Block::setup_output_ports()
{
	return false;
}

bool Block::setup_input_ports()
{
	return false;
}

InPort::store_t & Block::get_inport_list()
{
	return ports_in_;
}

OutPort::store_t & Block::get_outport_list()
{
	return ports_out_;
}

std::vector< Block::param_t > Block::get_params()
{
	return params_;
}
