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

#include <itpp/itbase.h>

using namespace plugboard;

class PlugBoardBlock : public Block, public Source
{
public:
	PlugBoardBlock();

private:
	void process();
	void initialize();

	void configure_parameters();
	void setup_output_ports();

	OutPort *bits_out_;
	itpp::ivec *i_vector_;

	int32_vec_t framesize_;
	real_vec_t Ts_;
	int32_vec_t hi_, lo_;

	itpp::I_Uniform_RNG prng;
};



PlugBoardBlock::PlugBoardBlock()
{
	set_name("RandomUniformBitGenerator");
	set_description("Creates uniformely distributed integer values.");
}



void PlugBoardBlock::setup_output_ports()
{
	bits_out_ = add_port(new OutPort("bits", int32, Ts_[0], framesize_[0]));
}



void PlugBoardBlock::initialize()
{
	prng.setup(lo_[0], hi_[0]);
	itpp::RNG_randomize();

	i_vector_ = get_signal< int32_t >(bits_out_);
}



void PlugBoardBlock::configure_parameters()
{
	add_parameter
	(
		(new Parameter(&Ts_, real, "Sample Time"))
		->add_constraint(new GreaterThanConstraint< real_t >(0.0))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&framesize_, int32, "Frame Size"))
		->add_constraint(new GreaterThanConstraint< int32_t >(0))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&lo_, int32, "Minimum value"))
		->add_constraint(new LessThanConstraint< int32_t >(0, true))
		->add_constraint(new SizeConstraint(1))
	);

	add_parameter
	(
		(new Parameter(&hi_, int32, "Maximum value"))
		->add_constraint(new LessThanConstraint< int32_t >(0, true))
		->add_constraint(new SizeConstraint(1))
	);
}



void PlugBoardBlock::process()
{
	*i_vector_ = prng(framesize_[0]);
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
	std::cout << " generated: " << *i_vector_ << std::endl;
#endif
}


#include "block/create.hpp"
