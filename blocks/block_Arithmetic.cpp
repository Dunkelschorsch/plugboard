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
#include <Eigen/Core>
#include <Eigen/Array>

#include "block/dynamic.hpp"
#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "block/sink.hpp"
#include "block/source.hpp"
#include "parameter.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <iostream>
#include <cstdio>



using namespace plugboard;

class PlugBoardBlock : public Block, public Source, public Sink, public Dynamic< PlugBoardBlock >
{
	PB_DYNAMIC_BLOCK

public:
	PlugBoardBlock();
	~PlugBoardBlock();

private:
	void configure_parameters();
	void setup_input_ports();
	void setup_output_ports();

	void initialize();

	template< typename T >
	void do_add();

	template< typename T >
	void do_mult();

	template< typename T >
	void do_sub();

	template< typename T >
	void do_div();

	OutPort *sig_out_;
	const InPort **sig_in_;

	void *v_out_;
	const void **v_in_;

	int32_vec_t num_inputs_;
	type_t input_type_;
	string_vec_t op_;

	typedef enum { ADD, MUL, SUB, DIV } ops;
	ops operation_;
};


PlugBoardBlock::PlugBoardBlock() : Dynamic< PlugBoardBlock >(this)
{
	set_name("Arithmetic");
	set_description("Perform basic arithmetic operations on an arbitrary, user-defined number of inputs.");
}


void PlugBoardBlock::configure_parameters( )
{
	add_parameter(&num_inputs_, "Number of inputs")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint< LessThanConstraint >(1000)
		->add_constraint(SizeConstraint(1));

	add_parameter(&op_, "Operation")
		->add_constraint(SizeConstraint(1));
}


void PlugBoardBlock::initialize( )
{
	if(op_[0] == "+")
		operation_ = ADD;
	else if(op_[0] == "-")
		operation_ = SUB;
	else if(op_[0] == "*")
		operation_ = MUL;
	else if(op_[0] == "/")
		operation_ = DIV;

	v_in_ = new const void* [num_inputs_];

	Dynamic< PlugBoardBlock >::initialize(sig_in_[0]);
}


void PlugBoardBlock::setup_input_ports()
{
	// only reserve that memory once
	if(get_num_input_ports() != num_inputs_)
		sig_in_ = new const InPort* [num_inputs_];

	for(int32_t i=0; i<num_inputs_; ++i)
	{
		char portname[5];
		std::sprintf(portname, "in%d", i+1);

		sig_in_[i] = add_port(new InPort(portname));
	}
}


void PlugBoardBlock::setup_output_ports()
{
	sig_out_ = add_port(new OutPort("result", sig_in_[0]->get_type(), sig_in_[0]->get_Ts(), sig_in_[0]->get_frame_size()));
}


template< typename T >
void PlugBoardBlock::dynamic_init()
{
	v_out_ = get_signal< T >(sig_out_);

	for(int32_t i=0; i<num_inputs_; ++i)
		v_in_[i] = get_signal< T >(sig_in_[i]);
}


template< typename T >
void PlugBoardBlock::do_add()
{
	typedef Eigen::Map< Eigen::Matrix< T, Eigen::Dynamic, 1 > > ei_vec;
	ei_vec m_out = ei_vec(static_cast< itpp::Vec<T>* >(v_out_)->_data(), static_cast< itpp::Vec<T>* >(v_out_)->length(), 1);

	for(int32_t i=1; i<num_inputs_; ++i)
	{
#ifndef NDEBUG
		std::cout << " in" << i+1 << ": " << *static_cast< const itpp::Vec<T>* >(v_in_[i]) << std::endl;
#endif
 		const ei_vec m_in = ei_vec(static_cast< const itpp::Vec<T>* >(v_in_[i])->_data(), static_cast< const itpp::Vec<T>* >(v_in_[i])->length(), 1);

		m_out += m_in;
	}
}


template< typename T >
void PlugBoardBlock::do_sub()
{
	typedef Eigen::Map< Eigen::Matrix< T, Eigen::Dynamic, 1 > > ei_vec;
	ei_vec m_out = ei_vec(static_cast< itpp::Vec<T>* >(v_out_)->_data(), static_cast< itpp::Vec<T>* >(v_out_)->length(), 1);

	for(int32_t i=1; i<num_inputs_; ++i)
	{
#ifndef NDEBUG
		std::cout << " in" << i+1 << ": " << *static_cast< const itpp::Vec<T>* >(v_in_[i]) << std::endl;
#endif
 		const ei_vec m_in = ei_vec(static_cast< const itpp::Vec<T>* >(v_in_[i])->_data(), static_cast< const itpp::Vec<T>* >(v_in_[i])->length(), 1);

		m_out -= m_in;
	}
}


template< typename T >
void PlugBoardBlock::do_mult()
{
	for(int32_t i=1; i<num_inputs_; ++i)
	{
#ifndef NDEBUG
		std::cout << " in" << i+1 << ": " << *static_cast< const itpp::Vec<T>* >(v_in_[i]) << std::endl;
#endif
		*static_cast< itpp::Vec<T>* >(v_out_) =
			elem_mult(*static_cast< itpp::Vec<T>* >(v_out_), *static_cast< const itpp::Vec<T>* >(v_in_[i]));
	}
}


template< typename T >
void PlugBoardBlock::do_div()
{
	for(int32_t i=1; i<num_inputs_; ++i)
	{
#ifndef NDEBUG
		std::cout << " in" << i+1 << ": " << *static_cast< const itpp::Vec<T>* >(v_in_[i]) << std::endl;
#endif
		*static_cast< itpp::Vec<T>* >(v_out_) =
			elem_div(*static_cast< itpp::Vec<T>* >(v_out_), *static_cast< const itpp::Vec<T>* >(v_in_[i]));
	}
}


template< typename T >
void PlugBoardBlock::dynamic_process()
{
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
	std::cout << " in1: " << *static_cast< const itpp::Vec<T>* >(v_in_[0]) << std::endl;
#endif
	*static_cast< itpp::Vec<T>* >(v_out_) = *static_cast< const itpp::Vec<T>* >(v_in_[0]);

	if(operation_ == ADD)
		do_add< T >();
	else if(operation_ == SUB)
		do_sub< T >();
	else if(operation_ == MUL)
		do_mult< T >();
	else if(operation_ == DIV)
		do_div< T >();
#ifndef NDEBUG
	std::cout << " out: " << *static_cast< itpp::Vec<T>* >(v_out_) << std::endl;
#endif
}


template< typename T >
void PlugBoardBlock::dynamic_delete()
{
	delete[] v_in_;
}


PlugBoardBlock::~PlugBoardBlock()
{
	if(is_configured())
		delete[] sig_in_;
}


#include "block/create.hpp"
