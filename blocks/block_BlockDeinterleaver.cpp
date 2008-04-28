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

#include <itpp/comm/interleave.h>
#include <itpp/itbase.h>


using namespace plugboard;


class PlugBoardBlock : public Block, public Sink, public Source, public Dynamic< PlugBoardBlock >
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
	const InPort* symbols_in_;
	const void *symbol_vector_in;
	
	OutPort* symbols_out_;
	void *symbol_vector_out;
	
	// Input parameters
	int32_vec_t framesize_, rows_, cols_;
	real_vec_t Ts_;

	void *interleaver_;
};


PlugBoardBlock::PlugBoardBlock() : Dynamic< PlugBoardBlock >(this)
{
	set_name("BlockDeinterleaver");
	set_description("Block Deinterleaver");
}


void PlugBoardBlock::configure_parameters()
{
	add_parameter(&Ts_, "Sample Time")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));

	add_parameter(&framesize_, "Frame Size")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));

	add_parameter(&rows_,"Rows")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint(SizeConstraint(1));

	add_parameter(&cols_, "Columns")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint(SizeConstraint(1));
}


void PlugBoardBlock::setup_input_ports()
{
	symbols_in_ = add_port(new InPort("in", empty, Ts_[0], framesize_[0]));
}


void PlugBoardBlock::setup_output_ports()
{
	symbols_out_ = add_port(new OutPort("out", symbols_in_->get_type(),
		symbols_in_->get_Ts(), symbols_in_->get_frame_size()));
}


void PlugBoardBlock::initialize()
{
	Dynamic< PlugBoardBlock >::initialize(symbols_in_);
}


template< typename T >
void PlugBoardBlock::dynamic_init()
{
	symbol_vector_in = get_signal< T >(symbols_in_);
	symbol_vector_out = get_signal< T >(symbols_out_);

	interleaver_ = new itpp::Block_Interleaver< T >(rows_[0], cols_[0]);
	static_cast< itpp::Block_Interleaver< T >* >(interleaver_)->interleave(itpp::Vec<T>(symbols_in_->get_frame_size()));
}


template< typename T >
void PlugBoardBlock::dynamic_process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl << " symbols_in(" << static_cast< const itpp::Vec<T>* >(symbol_vector_in)->size() << "): ";
	std::cout << *static_cast< const itpp::Vec<T>* >(symbol_vector_in) << std::endl;
#endif

	*static_cast< itpp::Vec<T>* >(symbol_vector_out) = static_cast< itpp::Block_Interleaver< T >* >(interleaver_)->deinterleave(*static_cast< const itpp::Vec<T>* >(symbol_vector_in));

#ifndef NDEBUG
	std::cout << " symbols_out(" << static_cast< itpp::Vec<T>* >(symbol_vector_out)->size() << "): ";
	std::cout << *static_cast< itpp::Vec<T>* >(symbol_vector_out) << std::endl;
#endif
}


template< typename T >
void PlugBoardBlock::dynamic_delete()
{
	delete static_cast< itpp::Block_Interleaver< T >* >(interleaver_);
}


#include "block/create.hpp"
