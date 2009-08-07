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
#include "block/sink.hpp"
#include "block/source.hpp"
#include "parameter.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <itpp/comm/modulator.h>

#ifndef NDEBUG
#include <iostream>
#endif

using namespace plugboard;

class PlugBoardBlock : public Block, public Sink, public Source
{
public:
	PlugBoardBlock();

private:
	void configure_parameters();
	void setup_input_ports();
	void setup_output_ports();

	void initialize();
	void process();

	// signals
	const InPort* bits_in_;
	const itpp::ivec *bit_vector_;

	OutPort* symbols_out_;
	itpp::cvec *symbol_vector_;

	// Input parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;

	int32_vec_t M_;

	itpp::PSK mod;

	uint8_t pad_bits_;
	itpp::ivec pad_vec_;

	itpp::bvec bits_to_modulate;
};


PlugBoardBlock::PlugBoardBlock()
{
	set_name("MPSKModulator");
	set_description("M-Ary PSK Modulator");
}


void PlugBoardBlock::configure_parameters()
{
	add_parameter(&Ts_, "Sample Time")
		->add_constraint< LessThanConstraint >(0.0, Constraint::reverse)
		->add_constraint(SizeConstraint(1));

	add_parameter(&framesize_, "Frame Size")
		->add_constraint< LessThanConstraint >(0, Constraint::reverse)
		->add_constraint(SizeConstraint(1));

	add_parameter(&M_, "M")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint(SizeConstraint(1));
}


void PlugBoardBlock::setup_input_ports()
{
	bits_in_ = add_port(new InPort("bits", int32, Ts_, framesize_));
}


void PlugBoardBlock::setup_output_ports()
{
	uint8_t bits_per_symbol = static_cast< unsigned int >(log2(M_));
	pad_bits_ = (bits_per_symbol - (bits_in_->get_frame_size() % bits_per_symbol)) % bits_per_symbol;

	bits_to_modulate.set_size(bits_in_->get_frame_size() + pad_bits_);
	bits_to_modulate.zeros();

	for(uint8_t i=0; i<pad_bits_; i++)
	{
		pad_vec_ = concat(pad_vec_, 0);
	}

	symbols_out_ = add_port(new OutPort("symbols", complex, bits_in_->get_Ts(),
		(bits_in_->get_frame_size()+pad_bits_)/bits_per_symbol));

}


void PlugBoardBlock::initialize()
{
	bit_vector_ = get_signal< int32_t >(bits_in_);
	symbol_vector_ = get_signal< complex_t >(symbols_out_);

	mod = itpp::PSK(M_);
}


void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl;
	print_vector_with_length("bits", bit_vector_);
#endif

	for(int i=0; i<bit_vector_->size(); i++)
	{
		bits_to_modulate(i)=(*bit_vector_)(i);
	}

	mod.modulate_bits(bits_to_modulate, *symbol_vector_);

#ifndef NDEBUG
	print_vector_with_length("symobls", symbol_vector_);
	assert_vector_sizes(symbol_vector_, symbols_out_);
#endif
}


#include "block/create.hpp"
