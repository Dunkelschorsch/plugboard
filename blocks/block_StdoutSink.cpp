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
#include <iostream>


using namespace plugboard;

class PlugBoardBlock : public Block, public Sink
{
public:
	PlugBoardBlock();

private:
	void setup_input_ports();

	void process();
	void initialize();

	template< typename T >
	void do_display();

	const InPort *sig_in_;
	const void *input_;

	void(PlugBoardBlock::*proc)();
};


void PlugBoardBlock::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in"));
}


void PlugBoardBlock::initialize( )
{
	if(sig_in_->get_type() == int32)
	{
		input_ = get_signal< int32_t >(sig_in_);
		proc = &PlugBoardBlock::do_display< int32_t >;
	}
	else if(sig_in_->get_type() == real)
	{
		input_ = get_signal< real_t >(sig_in_);
		proc = &PlugBoardBlock::do_display< real_t >;
	}
	else
	{
		input_ = get_signal< complex_t >(sig_in_);
		proc = &PlugBoardBlock::do_display< complex_t >;
	}
}


template< typename T >
void PlugBoardBlock::do_display()
{
	std::cout << *static_cast< const itpp::Vec<T>* >(input_) << std::endl;
}


void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl;
#endif
	std::cout << " ";
	(this->*proc)();
}


PlugBoardBlock::PlugBoardBlock()
{
	set_name("StdoutSink");
	set_description("Display input signal on standard output.");
#ifndef NDEBUG
	std::cout << "Contructed Block_" << get_name() << "!" << std::endl;
#endif
}


#include "block/create.hpp"
