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

#include <boost/random.hpp>
#include <boost/random/uniform_int.hpp>

#include <itpp/itbase.h>

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
	void finalize();

	// signals
	OutPort *bits_out_;

	itpp::ivec *i_vector_;
	int32_t* i_array_;

	int32_vec_t framesize_;
	real_vec_t Ts_;
	int32_vec_t hi_, lo_;

	// random number generation
	boost::mt19937 generator;
	boost::uniform_int< > uni_dist;
	boost::variate_generator< boost::mt19937&, boost::uniform_int< > > *make_bit;
};


PlugBoardBlock::PlugBoardBlock()
{
	set_name("RandomUniformBitGenerator");
	set_description("Creates uniformely distributed integer values.");
}


// PlugBoardBlock::~PlugBoardBlock()
// {
// 	delete make_bit;
// }


void PlugBoardBlock::configure_parameters()
{
	add_parameter(&Ts_,  "Sample Time")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint(SizeConstraint(1));

	add_parameter(&framesize_, "Frame Size")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint(SizeConstraint(1));

	add_parameter(&lo_, "Minimum value")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));

	add_parameter(&hi_, "Maximum value")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));
}


void PlugBoardBlock::setup_output_ports()
{
	bits_out_ = add_port(new OutPort("bits", int32, Ts_[0], framesize_[0]));
}


void PlugBoardBlock::initialize()
{
	uni_dist = boost::uniform_int< >(lo_[0], hi_[0]);

	make_bit = new boost::variate_generator< boost::mt19937&, boost::uniform_int< > >(generator, uni_dist);

	i_array_ = get_data< int32_t >(bits_out_);
	i_vector_ = get_signal< int32_t >(bits_out_);

}


void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
#endif
	uint32_t s = framesize_[0];
	for(uint32_t i=0; i < s; i++)
	{
		i_array_[i] = (*make_bit)();
	}

#ifndef NDEBUG
	std::cout << " generated: " << *i_vector_ << std::endl;
#endif
}


void PlugBoardBlock::finalize()
{
	delete make_bit;
}

#include "block/create.hpp"
