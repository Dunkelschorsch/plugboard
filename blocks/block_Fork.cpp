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

#include "block/dynamic.hpp"
#include "block/block.hpp"
#include "block/buffer_access.hpp"
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

private:
	void configure_parameters();
	void setup_input_ports();
	void setup_output_ports();

	void initialize();

	// signals
	OutPort **sig_out_;
	const InPort *sig_in1_;

	const void *v_in_;
	void **v_out_;

	int32_vec_t num_outputs_;
	type_t input_type_;
};


PlugBoardBlock::PlugBoardBlock() : Dynamic< PlugBoardBlock >(this)
{
	set_name("Fork");
	set_description("Clone the input to an arbitrary, user-defined number of outputs.");
}


void PlugBoardBlock::configure_parameters( )
{
	add_parameter(&num_outputs_, "Number of outputs")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint< LessThanConstraint >(1000)
		->add_constraint(SizeConstraint(1));
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


void PlugBoardBlock::initialize()
{
	Dynamic< PlugBoardBlock >::initialize(sig_in1_);
}


template< typename T >
void PlugBoardBlock::dynamic_init()
{
	v_out_ = new void* [num_outputs_[0]];

	v_in_ = get_signal< T >(sig_in1_);

	for(int32_t i=0; i<num_outputs_[0]; ++i)
		v_out_[i] = get_signal< T >(sig_out_[i]);
}


template< typename T >
void PlugBoardBlock::dynamic_process()
{
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
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


template< typename T >
void PlugBoardBlock::dynamic_delete()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << get_name() << "!" << std::endl;
#endif
	delete[] v_out_;
	delete[] sig_out_;
}


#include "block/create.hpp"
