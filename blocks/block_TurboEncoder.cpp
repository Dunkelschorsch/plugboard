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

#include <itpp/comm/turbo.h>

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

	// input signals
	const InPort* sig_in_;
	const itpp::ivec *in_vector_;

	// output signals
	OutPort* sig_out_;
	itpp::ivec *out_vector_;

	// block parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;

	// code parameters
	string_vec_t gen1_, gen2_;
	int32_vec_t constraint_length_;
	int32_vec_t interleaver_;

	// codec object
	itpp::Turbo_Codec code;

	uint32_t inv_rate_;
	uint32_t output_len_;
};


PlugBoardBlock::PlugBoardBlock()
{
	set_name("TurboEncoder");
	set_description("Encoder for Turbo Codes");
}


void PlugBoardBlock::configure_parameters()
{
	add_parameter(&Ts_, "Sample Time")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));

	add_parameter(&framesize_, "Frame Size")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));

	add_parameter(&gen1_, "Generator Polynomials for first constituent encoder")
		->add_constraint(SizeConstraint(1));

	add_parameter(&gen2_, "Generator Polynomials for second constituent encoder")
		->add_constraint(SizeConstraint(1));

	add_parameter(&constraint_length_, "Constraint Length")
		->add_constraint< GreaterThanConstraint >(1)
		->add_constraint(SizeConstraint(1));

	add_parameter(&interleaver_, "Internal Interleaver Sequence");
}


void PlugBoardBlock::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in", int32, Ts_[0], framesize_[0]));
}


void PlugBoardBlock::setup_output_ports()
{
	inv_rate_ = itpp::ivec(gen1_[0]).length() + (itpp::ivec(gen2_[0]).length()) - 1;
	output_len_ = sig_in_->get_frame_size() * inv_rate_ + (constraint_length_[0]-1) * (inv_rate_+1);

	sig_out_ = add_port(new OutPort("out", int32, sig_in_->get_Ts(), output_len_));
}


void PlugBoardBlock::initialize()
{
	in_vector_ = get_signal< int32_t >(sig_in_);
	out_vector_ = get_signal< int32_t >(sig_out_);

	itpp::ivec gen1 = gen1_[0];
	itpp::ivec gen2 = gen2_[0];
	itpp::ivec interleaver = itpp::wcdma_turbo_interleaver_sequence(sig_in_->get_frame_size());
	code.set_parameters(gen1, gen2, constraint_length_[0], interleaver);
#ifndef NDEBUG
	std::cout << this->get_name_sys() << " parameters: " << std::endl;
	std::cout << "first generator polynomial: " << gen1 << std::endl;
	std::cout << "second generator polynomial: " << gen2 << std::endl;
	std::cout << "constraint length: " << constraint_length_[0] << std::endl;
	std::cout << "rate: " << 1.0/inv_rate_ << std::endl;
	std::cout << "output_length: " << output_len_ << std::endl;
#endif
}


void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
	std::cout << " input(" << in_vector_->length() << "): " << *in_vector_ << std::endl;
#endif
	itpp::bvec out_tmp;
	code.encode(to_bvec(*in_vector_), out_tmp);
	*out_vector_ = to_ivec(out_tmp);
#ifndef NDEBUG
	std::cout << " coded(" << out_vector_->length() << "): " << *out_vector_ << std::endl;
#endif
}


#include "block/create.hpp"
