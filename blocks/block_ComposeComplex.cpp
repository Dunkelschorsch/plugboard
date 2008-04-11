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

#include <itpp/itbase.h>
#include <iostream>


using namespace plugboard;

class PlugBoardBlock : public Block, public Source, public Sink
{
public:
	PlugBoardBlock();

private:
	void setup_input_ports();
	void setup_output_ports();

	void process();
	void initialize();

	template< typename T >
	void do_compose();

	OutPort *complex_out_;
	const InPort *real_in_, *imag_in_;

	itpp::cvec *v_complex_;
	const void *v_real_, *v_imag_;

	type_t input_type_;
};



void PlugBoardBlock::setup_input_ports()
{
	real_in_ = add_port(new InPort("real", empty, 0, 0));
	imag_in_ = add_port(new InPort("imag", empty, 0, 0));
}



void PlugBoardBlock::setup_output_ports()
{
	complex_out_ = add_port(new OutPort("complex", complex, real_in_->get_Ts(), real_in_->get_frame_size()));
}


void PlugBoardBlock::initialize()
{
	input_type_ = real_in_->get_type();
	if(input_type_ == int32)
	{
		v_real_ = get_signal< int32_t >(real_in_);
		v_imag_ = get_signal< int32_t >(imag_in_);
	} else
	{
		v_real_ = get_signal< real_t >(real_in_);
		v_imag_ = get_signal< real_t >(imag_in_);
	}

	v_complex_ = get_signal< complex_t >(complex_out_);
}


template< typename T >
void PlugBoardBlock::do_compose()
{
	*v_complex_ = to_cvec(
		to_vec(*static_cast< const itpp::Vec<T>* >(v_real_)),
		to_vec(*static_cast< const itpp::Vec<T>* >(v_imag_))
	);
#ifndef NDEBUG
	std::cout << " real part: " << *static_cast< const itpp::Vec<T>* >(v_real_) << std::endl;
	std::cout << " imaginary part: " << *static_cast< const itpp::Vec<T>* >(v_imag_) << std::endl;
#endif
}


void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl;
#endif
	if(input_type_ == int32)
		do_compose< int32_t >();
	else
		do_compose< real_t >();

#ifndef NDEBUG
	std::cout << " complex: " << *v_complex_ << std::endl;
#endif
}


PlugBoardBlock::PlugBoardBlock()
{
	set_name("ComposeComplex");
	set_description("Create a complex signal out of two real valued signals.");
}


#include "block/create.hpp"
