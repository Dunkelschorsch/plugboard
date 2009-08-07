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



#include <itpp/comm/channel.h>
#include <itpp/itbase.h>

#include <cstring>
#include <tr1/functional>
#include <map>
#include <boost/preprocessor/stringize.hpp>

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
	const itpp::cvec *in_vector_;

	// output signals
	OutPort* sig_out_;
	itpp::cvec *out_vector_;

	// block parameters
	int32_vec_t framesize_;
	real_vec_t Ts_;
	real_vec_t channel_profile_sampling_frequency_;
	real_vec_t normalized_doppler_frequency_;
	string_vec_t channel_profile_;
	
	// channel object
	itpp::Channel_Specification channel_spec_;
	itpp::TDL_Channel channel_;

	#define NUM_PROFILES 21
	#define PROFILES (NUM_PROFILES, ( \
	ITU_Vehicular_A, \
	ITU_Vehicular_B, \
	ITU_Pedestrian_A, \
	ITU_Pedestrian_B, \
	COST207_RA, \
	COST207_RA6, \
	COST207_TU, \
	COST207_TU6alt, \
	COST207_TU12, \
	COST207_TU12alt, \
	COST207_BU, \
	COST207_BU6alt, \
	COST207_BU12, \
	COST207_BU12alt, \
	COST207_HT, \
	COST207_HT6alt, \
	COST207_HT12, \
	COST207_HT12alt, \
	COST259_TUx, \
	COST259_RAx, \
	COST259_HTx \
	))
	
	typedef std::map< std::string, itpp::Channel_Specification > channel_spec_map_t;
	channel_spec_map_t string_to_channel_spec_;
	
	uint32_t tail_length_;
	itpp::cvec tail_;
	itpp::cvec output_tmp_;
};


PlugBoardBlock::PlugBoardBlock()
{
	set_name("FadingChannel");
	set_description("Some fading channel...");
	
#define BOOST_PP_DEF(z, I, _) \
	string_to_channel_spec_.insert(std::make_pair(BOOST_PP_STRINGIZE(BOOST_PP_ARRAY_ELEM(I, PROFILES)), \
		itpp::BOOST_PP_ARRAY_ELEM(I, PROFILES)));
	BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)
#undef BOOST_PP_DEF
}


void PlugBoardBlock::configure_parameters()
{
	add_parameter(&Ts_, "Sample Time")
		->add_constraint< LessThanConstraint >(0, Constraint::reverse)
		->add_constraint(SizeConstraint(1));

	add_parameter(&framesize_, "Frame Size")
		->add_constraint< LessThanConstraint >(0, Constraint::reverse)
		->add_constraint(SizeConstraint(1));

	add_parameter(&normalized_doppler_frequency_, "Normalized Doppler Frequence")
		->add_constraint< GreaterThanConstraint >(0)
		->add_constraint< GreaterThanConstraint >(1, Constraint::reverse)
		->add_constraint(SizeConstraint(1));
		
	add_parameter(&channel_profile_sampling_frequency_, "Channel Profile Sampling Frequency")
		->add_constraint< LessThanConstraint >(0, Constraint::reverse)
		->add_constraint(SizeConstraint(1));
	
	add_parameter(&channel_profile_, "Channel Profile Name");
}


void PlugBoardBlock::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in", complex, Ts_, framesize_));
}


void PlugBoardBlock::setup_output_ports()
{
	sig_out_ = add_port(new OutPort("out", complex, sig_in_->get_Ts(), sig_in_->get_frame_size()));
}


void PlugBoardBlock::initialize()
{
	channel_spec_map_t::const_iterator pc = string_to_channel_spec_.find(channel_profile_);

	if(pc == string_to_channel_spec_.end())
	{
		std::string err_msg("No such channel profile: '" + std::string(channel_profile_) + "'");
		throw ConstraintTestException(err_msg);
	}
	
	channel_spec_ = itpp::Channel_Specification(pc->second);
	channel_ = itpp::TDL_Channel(channel_spec_, channel_profile_sampling_frequency_);
	channel_.set_norm_doppler(normalized_doppler_frequency_);

	channel_.filter(itpp::zeros_c(0), tail_);
	channel_.init();

	tail_length_ = tail_.size();

	in_vector_ = get_signal< complex_t >(sig_in_);
	out_vector_ = get_signal< complex_t >(sig_out_);
}


void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl;
	print_vector_with_length("symbols in", in_vector_);
#endif

	channel_.filter(*in_vector_, output_tmp_);

	*out_vector_ = output_tmp_(0, out_vector_->size()-1);
	out_vector_->set_subvector(0, tail_length_-1, (*out_vector_)(0, tail_length_-1) + tail_);

	tail_ = output_tmp_(out_vector_->size(), -1);
#ifndef NDEBUG
	print_vector_with_length("symbols out", out_vector_);
	print_vector_with_length("tail", &tail_);
#endif
}

#include "block/create.hpp"
