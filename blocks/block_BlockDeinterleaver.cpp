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

#include <itpp/comm/interleave.h>
#include <itpp/itbase.h>


using namespace plugboard;

class PlugBoardBlock : public Block, public Sink, public Source
{
public:
	PlugBoardBlock();

private:
	void configure_parameters();
	void setup_input_ports();
	void setup_output_ports();

	void process();
	void initialize();

	const InPort* symbols_in_;
	const itpp::ivec *symbol_vector_in_;

	OutPort* symbols_out_;
	itpp::ivec *symbol_vector_out_;

	// Input parameters
	int32_vec_t framesize_, rows_, cols_;
	real_vec_t Ts_;

	itpp::Block_Interleaver< int32_t > interleaver_;
};


PlugBoardBlock::PlugBoardBlock()
{
	set_name("BlockDeinterleaver");
	set_description("Block Deinterleaver");
}


void PlugBoardBlock::setup_input_ports()
{
	symbols_in_ = add_port(new InPort("in", int32, Ts_[0], framesize_[0]));
}


void PlugBoardBlock::setup_output_ports()
{
	symbols_out_ = add_port(new OutPort("out", int32, symbols_in_->get_Ts(),
		symbols_in_->get_frame_size()));
}


void PlugBoardBlock::initialize()
{
	symbol_vector_in_ = get_signal< int32_t >(symbols_in_);
	symbol_vector_out_ = get_signal< int32_t >(symbols_out_);

	interleaver_.set_rows(rows_[0]);
	interleaver_.set_cols(cols_[0]);
	interleaver_.interleave(itpp::ivec(symbols_in_->get_frame_size()));
}


void PlugBoardBlock::configure_parameters()
{
	add_parameter
	(
		(new Parameter(&Ts_, real, "Sample Time"))
		->add_constraint(new LessThanConstraint< real_t >(0.0, true))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&framesize_, int32, "Frame Size"))
		->add_constraint(new LessThanConstraint< int32_t >(0, true))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&rows_, int32, "Rows"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&cols_, int32, "Columns"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new SizeConstraint(1))
	);
}


void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl << " symbols_in(" << symbol_vector_in_->size() << "): ";
	std::cout << *symbol_vector_in_ << std::endl;
#endif

	*symbol_vector_out_ = interleaver_.deinterleave(*symbol_vector_in_);

#ifndef NDEBUG
	std::cout << " symbols_out(" << symbol_vector_out_->size() << "): ";
	std::cout << *symbol_vector_out_ << std::endl;
#endif
}

#include "block/create.hpp"
