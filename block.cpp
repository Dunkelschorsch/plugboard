#include "block.hpp"
#include "port.hpp"
#include "variable.hpp"
#include "exceptions.hpp"

#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <iostream>



Block::Block() :
	parameter_factory_(),
	params_(),
	param_curr_(0),
	configured_(false),
	name_(),
	description_(),
	ports_in_(),
	ports_out_(),
	name_sys_(),
	num_output_ports_(0),
	num_input_ports_(0)
{
	register_parameter_types();
}



void Block::register_parameter_types()
{
	// integer_t
	parameter_factory_.insert(std::make_pair(integer, boost::bind(&Block::copy_parameter< integer_t >, this, _1, _2)));

	// real_t
	parameter_factory_.insert(std::make_pair(real, boost::bind(&Block::copy_parameter< real_t >, this, _1, _2)));

	// complex_t
	parameter_factory_.insert(std::make_pair(complex, boost::bind(&Block::copy_parameter< complex_t >, this, _1, _2)));

	// string_t
	parameter_factory_.insert(std::make_pair(string, boost::bind(&Block::copy_parameter< string_t >, this, _1, _2)));

	// logical_t
	parameter_factory_.insert(std::make_pair(logical, boost::bind(&Block::copy_parameter< logical_t >, this, _1, _2)));
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



template < typename T >
void Block::copy_parameter(void *out, Variable& p)
{
	for(Variable::iterator it = p.begin(); it != p.end(); it++)
	{
		reinterpret_cast< std::vector < T >* >(out)->push_back(boost::any_cast< T >(*it));
	}
	param_curr_++;
}



bool Block::set_parameter(const Variable& pp)
{
	/* FIXME This is evil! Implement a const_iterator instead! */
	Variable p = const_cast< Variable& >(pp);

	if (is_configured())
	{
		std::cout << "Block is already configured." << std::endl;
		return false;
	}

	boost::function< void(void*, Variable&) > f = parameter_factory_[p.type()];

	if (params_[param_curr_].second == p.type())
	{
		f(params_[param_curr_].first, p);
		if (params_.size() == param_curr_)
		{
			configured_ = true;
		}
	}
	return true;
}


OutPort* Block::add_port(const OutPort& p)
{
	OutPort::store_t::iterator it =
		std::find_if(ports_out_.begin(), ports_out_.end(), boost::bind(&BasePort::get_name, _1) == p.get_name());

	if (it != ports_out_.end())
	{
		throw DuplicatePortNameException(get_name() + "::" + p.get_name());
	}
	ports_out_.push_back(p);

	num_output_ports_++;
	return &(*(ports_out_.end()-1));
}


InPort* Block::add_port(const InPort& p)
{
	InPort::store_t::iterator it =
		std::find_if(ports_in_.begin(), ports_in_.end(), boost::bind(&BasePort::get_name, _1) == p.get_name());

	if (it != ports_in_.end())
	{
		throw DuplicatePortNameException(get_name() + "::" + p.get_name());
	}
	ports_in_.push_back(p);

	num_input_ports_++;
	return &(*(ports_in_.end()-1));
}


Block::~Block()
{

}


void Block::set_name_sys(const std::string & name_sys)
{
	name_sys_ = name_sys;
}


std::string Block::get_name_sys() const
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



uint16_t Block::get_num_input_ports() const
{
	return num_input_ports_;
}



uint16_t Block::get_num_output_ports() const
{
	return num_output_ports_;
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
