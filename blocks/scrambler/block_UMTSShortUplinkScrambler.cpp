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

#include "scramble.hpp"

using namespace plugboard;

class PlugBoardBlock : public Block, public Source
{
public:
	PlugBoardBlock();

private:
	void configure_parameters();
	void setup_output_ports();

	void initialize();
	void process();

	// signals
	OutPort *scr_out_;
	itpp::cvec *c_vector_;

	int32_vec_t framesize_;
	real_vec_t Ts_;

	int32_vec_t sequence_length_, sequence_number_;

	itpp::cvec Z;

	typedef ShortUplinkScrambler scrambler_t;
	scrambler_t s;
};


PlugBoardBlock::PlugBoardBlock()
{
	set_name("Scrambler");
	set_description("Creates UMTS Scrambling sequences");
}


void PlugBoardBlock::configure_parameters()
{
	add_parameter(&Ts_,  "Sample Time")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint(SizeConstraint(1));

	add_parameter(&framesize_, "Frame Size")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint(SizeConstraint(1));

	add_parameter(&sequence_length_, "Scrambling sequence length")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint(SizeConstraint(1));

	add_parameter(&sequence_number_, "Scrambling sequence number")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));
}


void PlugBoardBlock::setup_output_ports()
{
	scr_out_ = add_port(new OutPort("out", complex, Ts_[0], framesize_[0]));
}


void PlugBoardBlock::initialize()
{
	c_vector_ = get_signal< complex_t >(scr_out_);

	s = scrambler_t(sequence_number_[0]);
	s.generate();
}


void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
#endif

	*c_vector_ = s.get_scrambling_sequence();
#ifndef NDEBUG

	std::cout << " generated: " << *c_vector_ << std::endl;
#endif
}


#include "block/create.hpp"
