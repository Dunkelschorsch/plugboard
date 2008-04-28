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
	OutPort* bits_out_;
	void *bits_v_;

	const InPort* symbols_in_;
	const itpp::cvec *symbol_vector_;

	// Input parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;

	int32_vec_t M_;
	int32_vec_t soft_demod_;

	itpp::PSK mod;
};



PlugBoardBlock::PlugBoardBlock()
{
	set_name("MPSKDemodulator");
	set_description("M-Ary PSK Demodulator");
}


void PlugBoardBlock::configure_parameters()
{
	add_parameter(&Ts_, "Sample Time")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));

	add_parameter(&framesize_, "Frame Size")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));

	add_parameter(&M_, "M")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint(SizeConstraint(1));

	add_parameter(&soft_demod_, "Soft Demodulation")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));
}


void PlugBoardBlock::setup_input_ports()
{
	symbols_in_ = add_port(new InPort("symbols", complex, Ts_[0], framesize_[0]));
}


void PlugBoardBlock::setup_output_ports()
{
	if(soft_demod_[0] == 1)
		bits_out_ = add_port(new OutPort("bits", real, symbols_in_->get_Ts(), static_cast< unsigned int >(log2(M_[0]))*symbols_in_->get_frame_size()));
	else
		bits_out_ = add_port(new OutPort("bits", int32, symbols_in_->get_Ts(), static_cast< unsigned int >(log2(M_[0]))*symbols_in_->get_frame_size()));
}


void PlugBoardBlock::initialize()
{
	if(soft_demod_[0] == 1)
		bits_v_ = get_signal< real_t >(bits_out_);
	else
		bits_v_ = get_signal< int32_t >(bits_out_);

	symbol_vector_ = get_signal< complex_t >(symbols_in_);

	mod = itpp::PSK(M_[0]);
}


void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl;
	std::cout << " input(" << symbol_vector_->size() << "): " << *symbol_vector_ << std::endl;
	std::cout << " output(";
#endif
	if(soft_demod_[0] == 1)
	{
		mod.demodulate_soft_bits(*symbol_vector_, 1, *static_cast< itpp::Vec< real_t >* >(bits_v_));
#ifndef NDEBUG

		std::cout << static_cast< itpp::Vec< real_t >* >(bits_v_)->size() << "): " <<
			*static_cast< itpp::Vec< real_t >* >(bits_v_) << std::endl;
#endif
	}
	else {
		*static_cast< itpp::Vec< int32_t >* >(bits_v_) = to_ivec(mod.demodulate_bits(*symbol_vector_));
#ifndef NDEBUG

		std::cout << static_cast< itpp::Vec< int32_t >* >(bits_v_)->size() << "): " <<
			*static_cast< itpp::Vec< int32_t >* >(bits_v_) << std::endl;
#endif
	}
}


#include "block/create.hpp"
