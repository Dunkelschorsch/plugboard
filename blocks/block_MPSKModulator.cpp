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

#include <itpp/comm/modulator.h>
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

	const InPort* bits_in_;
	const itpp::ivec *bit_vector_;

	OutPort* symbols_out_;
	itpp::cvec *symbol_vector_;

	// Input parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;

	int32_vec_t M_;
	itpp::PSK mod;
};



PlugBoardBlock::PlugBoardBlock()
{
	set_name("MPSKModulator");
	set_description("M-Ary PSK Modulator");
}



void PlugBoardBlock::setup_input_ports()
{
	bits_in_ = add_port(new InPort("bits", int32, Ts_[0], framesize_[0]));
}



void PlugBoardBlock::setup_output_ports()
{
	symbols_out_ = add_port(new OutPort("symbols", complex, bits_in_->get_Ts(),
		bits_in_->get_frame_size()/static_cast< unsigned int >(log2(M_[0]))));
}



void PlugBoardBlock::initialize()
{
	bit_vector_ = get_signal< int32_t >(bits_in_);
	symbol_vector_ = get_signal< complex_t >(symbols_out_);

	mod = itpp::PSK(M_[0]);
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
		(new Parameter(&M_, int32, "M"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new SizeConstraint(1))
	);
}



void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl << " bits(" << bit_vector_->size() << "): ";
	std::cout << *bit_vector_ << std::endl;
#endif

	mod.modulate_bits(to_bvec(*bit_vector_), *symbol_vector_);

#ifndef NDEBUG
	std::cout << " symobls(" << symbol_vector_->size() << "): ";
	std::cout << *symbol_vector_ << std::endl;
#endif	
}


#include "block/create.hpp"
