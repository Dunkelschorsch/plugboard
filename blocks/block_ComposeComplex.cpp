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

#include "block/dynamic.hpp"
#include "block/block.hpp"
#include "block/buffer_access.hpp"
#include "types/base.hpp"

#include <itpp/itbase.h>
#include <iostream>


using namespace plugboard;

class PlugBoardBlock : public Block, public Source, public Sink, public Dynamic< PlugBoardBlock >
{
	PB_DYNAMIC_BLOCK

public:
	PlugBoardBlock();

private:
	void setup_input_ports();
	void setup_output_ports();

	void initialize();

	OutPort *complex_out_;
	const InPort *real_in_, *imag_in_;

	itpp::cvec *v_complex_;
	const void *v_real_, *v_imag_;
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


template< typename T >
void PlugBoardBlock::dynamic_init()
{
	v_real_ = get_signal< T >(real_in_);
	v_imag_ = get_signal< T >(imag_in_);
}


void PlugBoardBlock::initialize()
{
	v_complex_ = get_signal< complex_t >(complex_out_);
	Dynamic< PlugBoardBlock >::initialize< boost::mpl::set< int32_t, real_t > >(real_in_);
}


template< typename T >
void PlugBoardBlock::dynamic_process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl;
#endif
	*v_complex_ = to_cvec(
		to_vec(*static_cast< const itpp::Vec<T>* >(v_real_)),
		to_vec(*static_cast< const itpp::Vec<T>* >(v_imag_))
	);
#ifndef NDEBUG
	std::cout << " real part: " << *static_cast< const itpp::Vec<T>* >(v_real_) << std::endl;
	std::cout << " imaginary part: " << *static_cast< const itpp::Vec<T>* >(v_imag_) << std::endl;
	std::cout << " complex: " << *v_complex_ << std::endl;
#endif
}


PlugBoardBlock::PlugBoardBlock() : Dynamic< PlugBoardBlock >(this)
{
	set_name("ComposeComplex");
	set_description("Create a complex signal out of two real valued signals.");
}


template< typename T >
void PlugBoardBlock::dynamic_delete() { }


#include "block/create.hpp"
