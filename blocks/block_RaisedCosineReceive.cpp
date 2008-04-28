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
#include <itpp/itstat.h>
#include <itpp/signal/filter.h>

using namespace plugboard;


template< typename T1, typename T2 >
class Matched_Filter : public itpp::Filter< T1, T2, T1 >
{
public:
    Matched_Filter(itpp::Vec<T2> coeffs_in, int downsample_factor_in) :
        coeffs(coeffs_in),
        downsample_factor(downsample_factor_in),
	inptr(0)
    {
        mem.set_size(coeffs.size(), false) ;
        mem.clear();
    }

    T1 filter(T1 const sample)
    {
        T1 output = 0;
        mem(inptr) = sample;

        int L = mem.length() - inptr;

        for (int i = 0; i < L; i++)
        {
            output += coeffs(i) * mem(inptr + i);
        }

        for (int i = 0; i < inptr; i++) {
            output += coeffs(L + i) * mem(i);
        }

        inptr--;
        if (inptr < 0)
            inptr += mem.length();

        return output;
    }

    itpp::Vec<T1> operator()(itpp::Vec<T1> const& v)
    {
        itpp::Vec<T1> output(v.size()/downsample_factor);
        for(int i=0; i<output.size(); i++)
        {
               output(i) = filter(v(i*downsample_factor));

               for(unsigned int j=1; j<downsample_factor; j++)
               {
                    put_sample(v(i*downsample_factor + j));
               }
        }
        return output;
    }

private:
    const itpp::Vec<T2> coeffs;
    itpp::Vec<T1> mem;

    const unsigned int downsample_factor;
    int inptr;

    void put_sample(T1 const sample)
    {
        mem(inptr) = sample;

        inptr--;
        if (inptr < 0)
            inptr += mem.length();
    }
};



class PlugBoardBlock : public Block, public Sink, public Source
{
public:
	PlugBoardBlock();
	~PlugBoardBlock();
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
	real_vec_t alpha_;
	int32_vec_t filter_length_;
	int32_vec_t downsampling_factor_;

	// pulse shaper object
	itpp::Root_Raised_Cosine< complex_t > *rc;
	// filter object
	Matched_Filter< complex_t, double > *mf;
};



PlugBoardBlock::PlugBoardBlock()
{
	set_name("RaisedCosineReceive");
	set_description("Raised cosine receive filter");
}


void PlugBoardBlock::configure_parameters()
{
	add_parameter(&Ts_,"Sample Time")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));

	add_parameter(&framesize_, "Frame Size")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));

	add_parameter(&alpha_, "Roll-off factor")
		->add_constraint< LessThanConstraint >(0, true)
		->add_constraint(SizeConstraint(1));

	add_parameter(&filter_length_, "Filter length")
		->add_constraint< ModuloConstraint >(2)
		->add_constraint(SizeConstraint(1));

	add_parameter(&downsampling_factor_, "Downsampling factor")
		->add_constraint< GreaterThanConstraint >(1)
		->add_constraint(SizeConstraint(1));
}


void PlugBoardBlock::setup_input_ports()
{
	sig_in_ = add_port(new InPort("in", complex, Ts_[0], framesize_[0]));
}


void PlugBoardBlock::setup_output_ports()
{
	sig_out_ = add_port(new OutPort("out", complex,
		sig_in_->get_Ts() * downsampling_factor_[0],
		sig_in_->get_frame_size() / downsampling_factor_[0])
	);
}


void PlugBoardBlock::initialize()
{
	in_vector_ = get_signal< complex_t >(sig_in_);
	out_vector_ = get_signal< complex_t >(sig_out_);

	framesize_[0] = sig_in_->get_frame_size();

	rc = new itpp::Root_Raised_Cosine< complex_t >(alpha_[0], filter_length_[0], downsampling_factor_[0]);
	mf = new Matched_Filter< complex_t, double >(rc->get_pulse_shape(), downsampling_factor_[0]);
	delete rc;
}


void PlugBoardBlock::process()
{
#ifndef NDEBUG
	std::cout << get_name_sys() << std::endl << " samples in(" << in_vector_->size() << "): " ;
	std::cout << "rms: " << sqrt(itpp::mean(itpp::sqr(*in_vector_))) << " ";
	std::cout << *in_vector_ << std::endl;
#endif

        *out_vector_ = (*mf)(*in_vector_);

#ifndef NDEBUG
	std::cout << " samples out(" << out_vector_->size() << "): " ;
	std::cout << "rms: " << sqrt(itpp::mean(itpp::sqr(*out_vector_))) << " ";
	std::cout << *out_vector_ << std::endl;
#endif
}


PlugBoardBlock::~PlugBoardBlock()
{
	if(is_initialized())
		delete mf;
}


#include "block/create.hpp"
