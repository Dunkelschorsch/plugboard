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

#include <itpp/itcomm.h>


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
	real_vec_t code_rate_;
	int32_vec_t constraint_length_;
	int32_vec_t code_type_;

	// codec object
	itpp::Convolutional_Code code;
};


PlugBoardBlock::PlugBoardBlock()
{
	set_name("ConvolutionalEncoder");
	set_description("Encoder for Convolutional Codes");
}


void PlugBoardBlock::configure_parameters()
{
	add_parameter(&Ts_, "Sample Time")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));

	add_parameter(&framesize_, "Frame Size")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));

	add_parameter(&code_rate_, "Code Rate")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint(SizeConstraint(1));

	add_parameter(&constraint_length_, "Constraint Length")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint(SizeConstraint(1));

	add_parameter(&code_type_, "Code Type")
		->add_constraint(SizeConstraint(1));
}


void PlugBoardBlock::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in", int32, Ts_[0], framesize_[0]));
}


void PlugBoardBlock::setup_output_ports()
{
	sig_out_ = add_port(new OutPort("out", int32, sig_in_->get_Ts(),
		static_cast< int32_t >(1/code_rate_[0])
		* ( sig_in_->get_frame_size() + constraint_length_[0]-1 ) ));
}


void PlugBoardBlock::initialize()
{
	in_vector_ = get_signal< int32_t >(sig_in_);
	out_vector_ = get_signal< int32_t >(sig_out_);

	code = itpp::Convolutional_Code();
	code.set_method(itpp::Tail);

	itpp::CONVOLUTIONAL_CODE_TYPE type = code_type_[0] == 0 ? itpp::MFD : itpp::ODS;

#ifndef NDEBUG
	std::cout << this->get_name_sys() << " parameters: " << std::endl;
	std::cout << "inverse rate: " << static_cast< int32_t >(1/code_rate_[0]) << std::endl;
	std::cout << "constraint length: " << constraint_length_[0] << std::endl;
#endif
	code.set_code(type, static_cast< int32_t >(1/code_rate_[0]), constraint_length_[0]);
}


void PlugBoardBlock::process()
{

#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
	std::cout << " input: " << *in_vector_ << std::endl;
#endif

	*out_vector_ = to_ivec(code.encode(to_bvec(*in_vector_)));

#ifndef NDEBUG
	std::cout << " coded: " << *out_vector_ << std::endl;
#endif
}


#include "block/create.hpp"
