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
#include <iostream>

#include <itpp/comm/error_counters.h>
#include <itpp/itbase.h>

using namespace plugboard;

class PlugBoardBlock : public Block, public Sink
{
public:
	PlugBoardBlock();

private:
	void configure_parameters();
	void setup_input_ports();
	void initialize();

	void process();
	void finalize();

	// signals
	const InPort *tx_in_, *rx_in_;
	const itpp::Vec< int32_t > *tx_vec, *rx_vec;

	itpp::BERC berc;

	std::vector< std::string > filename_;
	int32_vec_t delay_;

	itpp::it_file report_file;
};


PlugBoardBlock::PlugBoardBlock()
{
	set_name("BitErrorCount");
	set_description("Counts bit errors between the two input signals");
}


void PlugBoardBlock::configure_parameters()
{
	add_parameter(&filename_, "Filename to save")
		->add_constraint(SizeConstraint(1));

	add_parameter(&delay_, "Delay")
		->add_constraint(SizeConstraint(1));
}


void PlugBoardBlock::setup_input_ports()
{
	tx_in_ = add_port(new InPort("tx", int32, 0, 0));
	rx_in_ = add_port(new InPort("rx", int32, 0, 0));
}


void PlugBoardBlock::initialize()
{
	report_file.open(filename_[0]);

	tx_vec = get_signal< int32_t >(tx_in_);
	rx_vec = get_signal< int32_t >(rx_in_);

	berc = itpp::BERC(delay_[0]);
}


void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl;

	std::cout << " rx: " << *rx_vec << std::endl;
	std::cout << " tx: " << *tx_vec << std::endl;
#endif
	berc.count(to_bvec(*tx_vec), to_bvec(*rx_vec));
}


void PlugBoardBlock::finalize()
{
#ifndef NDEBUG
	std::cout << "Writing BER report to file '" << filename_[0] << "'" << std::endl;
#endif
	using itpp::Name;
	report_file
		<< Name("bits_total", "total number of transmitted bits") << berc.get_total_bits()
		<< Name("bit_errors", "number of bit errrors") << berc.get_errors()
		<< Name("bit_corrects", "number correctly transmitted bits") << berc.get_corrects()
		<< Name("bit_error_rate", "bit error rate") << berc.get_errorrate()
	;
	report_file.flush();
#ifndef NDEBUG
	berc.report();
#endif
	report_file.close();
}


#include "block/create.hpp"
