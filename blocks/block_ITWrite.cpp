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
#include "block/sink.hpp"
#include "parameter.hpp"
#include "types/base.hpp"
#include "types/vectors.hpp"
#include "constraint.hpp"

#include <itpp/itbase.h>

#ifndef NDEBUG
#include <iostream>
#endif

using namespace plugboard;

class PlugBoardBlock : public Block, public Sink, public Dynamic< PlugBoardBlock >
{
	PB_DYNAMIC_BLOCK

public:
	PlugBoardBlock();

private:
	void configure_parameters();
	void setup_input_ports();

	void initialize();

	// signals
	const InPort *sig_in1_;
	const void *v_in_;

	type_t input_type_;

	std::vector< std::string > filename_;
	itpp::it_file file_;
};


PlugBoardBlock::PlugBoardBlock() : Dynamic< PlugBoardBlock >(this)
{
	set_name("ITWrite");
	set_description("Write input signal to an .it file.");
}


void PlugBoardBlock::configure_parameters( )
{
	add_parameter(&filename_, "Filename to save")
		->add_constraint(SizeConstraint(1));
}


void PlugBoardBlock::setup_input_ports()
{
	sig_in1_ = add_port(new InPort("in"));
}



void PlugBoardBlock::initialize()
{
	Dynamic< PlugBoardBlock >::initialize(sig_in1_);
	file_.open(filename_[0]);
}


template< typename T >
void PlugBoardBlock::dynamic_init()
{
	v_in_ = get_signal< T >(sig_in1_);
	
}


template< typename T >
void PlugBoardBlock::dynamic_process()
{
#ifndef NDEBUG
	std::cout << this->get_name_sys() << std::endl;
	std::cout << " in:   " << *static_cast< const itpp::Vec<T>* >(v_in_) << std::endl;
#endif
	using itpp::Name;
	file_ << Name("data", "Data") << *static_cast< const itpp::Vec<T>* >(v_in_);
}


template< typename T >
void PlugBoardBlock::dynamic_delete()
{
#ifndef NDEBUG
	std::cout << "Bye from Block_" << get_name() << "!" << std::endl;
#endif
	file_.flush();
	file_.close();
}


#include "block/create.hpp"
