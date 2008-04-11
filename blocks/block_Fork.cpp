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

#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <iostream>
#include <cstdio>


using namespace plugboard;

class PlugBoardBlock : public Block, public Source, public Sink
{
public:

	PlugBoardBlock();
	~PlugBoardBlock();

private:
	void setup_input_ports();
	void setup_output_ports();
	void configure_parameters();

	void initialize();
	void process();

	template< typename T >
	void do_fork();

	template< typename T >
	void do_init();

	OutPort **sig_out_;
	const InPort *sig_in1_;

	const void *v_in_;
	void **v_out_;

	int32_vec_t num_outputs_;
	type_t input_type_;

	void (PlugBoardBlock::*proc)();
};


void PlugBoardBlock::configure_parameters( )
{
	add_parameter
	(
		(new Parameter(&num_outputs_, int32, "Number of outputs"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new LessThanConstraint< int32_t >(1000))
		->add_constraint(new SizeConstraint(1))
	);
}


void PlugBoardBlock::setup_input_ports()
{
	sig_in1_ = add_port(new InPort("in"));
}


void PlugBoardBlock::setup_output_ports()
{
	// only reserve that memory once
	if(get_num_output_ports() != num_outputs_[0])
		sig_out_ = new OutPort* [num_outputs_[0]];

	for(int32_t i=0; i<num_outputs_[0]; ++i)
	{
		char portname[6];
		std::sprintf(portname, "out%d", i+1);

		sig_out_[i] = add_port(new OutPort(portname,
			sig_in1_->get_type(), sig_in1_->get_Ts(), sig_in1_->get_frame_size()));
	}
}


template< typename T >
void PlugBoardBlock::do_init()
{
	v_in_ = get_signal< T >(sig_in1_);
	proc = &PlugBoardBlock::do_fork< T >;

	for(int32_t i=0; i<num_outputs_[0]; ++i)
		v_out_[i] = get_signal< T >(sig_out_[i]);
}


void PlugBoardBlock::initialize()
{
	v_out_ = new void* [num_outputs_[0]];
	input_type_ = sig_in1_->get_type();

	if(input_type_ == int32)
		do_init< int32_t >();
	if(input_type_ == real)
		do_init< real_t >();
	if(input_type_ == complex)
		do_init< complex_t >();
}


template< typename T >
void PlugBoardBlock::do_fork()
{
#ifndef NDEBUG
		std::cout << " in:   " << *static_cast< const itpp::Vec<T>* >(v_in_) << std::endl;
#endif
		for(int32_t i=0; i<num_outputs_[0]; ++i)
		{
			*static_cast< itpp::Vec<T>* >(v_out_[i]) =
			*static_cast< const itpp::Vec<T>* >(v_in_);
#ifndef NDEBUG
			std::cout << " out" << i+1 << ": " << *static_cast< itpp::Vec<T>* >(v_out_[i]) << std::endl;
#endif
			assert(*static_cast< itpp::Vec<T>* >(v_out_[i]) ==
				*static_cast< const itpp::Vec<T>* >(v_in_));
		}
}


void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
#endif
	(this->*proc)();
}


PlugBoardBlock::PlugBoardBlock()
{
	set_name("Fork");
	set_description("Clone the input to an arbitrary, user-defined number of outputs.");
}


PlugBoardBlock::~PlugBoardBlock()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << get_name() << "!" << std::endl;
#endif
	if(is_initialized())
		delete[] v_out_;
	if(is_configured())
		delete[] sig_out_;
}


#include "block/create.hpp"
