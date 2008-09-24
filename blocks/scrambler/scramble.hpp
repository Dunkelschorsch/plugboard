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

#ifndef SCRAMBLE_HPP
#define SCRAMBLE_HPP

#include "visibility.hpp"
#include "types/base.hpp"
#include <itpp/itbase.h>


template< class DerivedScrambler >
struct Scrambler
{
	void generate()
	{
		static_cast< DerivedScrambler* >(this)->generate_impl();
	}

	const itpp::Vec< complex_t >& get_scrambling_sequence() const
	{
		return Z_;
	}

	const itpp::Vec< int8_t >& get_scrambling_sequence_re() const
	{
		return c_n_real_;
	}

	const itpp::Vec< int8_t >& get_scrambling_sequence_im() const
	{
		return c_n_imag_;
	}

	const itpp::Vec< complex_t > get_scrambling_sequence(uint32_t samples)
	{
		if(start_output_ + samples > seq_len_)
		{
			itpp::Vec< complex_t > z(Z_(start_output_, -1));
			uint32_t samples_left = samples - (seq_len_ - start_output_ + 1);
			start_output_ = 0;
			return itpp::concat(z, this->get_scrambling_sequence(samples_left));
		}
		else
		{
			itpp::Vec< complex_t > z(Z_(start_output_, start_output_ + samples-1));
			start_output_ += samples;
			return z;
		}
	}

protected:
	Scrambler() : seq_num_(0), seq_len_(0), start_output_(0) { }

	Scrambler(uint32_t seq_len, uint32_t seq_num) :
		seq_num_(seq_num),
		seq_len_(seq_len),
		start_output_(0),
		c_n_real_(itpp::Vec< int8_t >(seq_len_)),
		c_n_imag_(itpp::Vec< int8_t >(seq_len_)),
		c_1_n_(itpp::Vec< int8_t >(seq_len_)),
		c_2_n_(itpp::Vec< int8_t >(seq_len_))
	{ }

	template< typename T >
	inline T LSB(const T p) const
	{
		return LnSB<1>(p);
	}

	template< uint8_t n, typename T >
	inline T LnSB(const T p) const
	{
		return p & ((2 << (n - 1)) - 1);
	}

	uint32_t seq_num_, seq_len_, start_output_;

	itpp::Vec< int8_t > c_n_real_;
	itpp::Vec< int8_t > c_n_imag_;

	itpp::Vec< int8_t > c_1_n_;
	itpp::Vec< int8_t > c_2_n_;

	itpp::Vec< complex_t > Z_;
};



class DownlinkScrambler : public Scrambler< DownlinkScrambler >
{
friend class Scrambler< DownlinkScrambler >;
public:
	DownlinkScrambler() : Scrambler< DownlinkScrambler >() { }

	DownlinkScrambler(uint32_t seq_num);

private:
	void generate_impl();
};


class ShortUplinkScrambler : public Scrambler< ShortUplinkScrambler >
{
friend class Scrambler< ShortUplinkScrambler >;
public:
	ShortUplinkScrambler() : Scrambler< ShortUplinkScrambler >() { }

	ShortUplinkScrambler(uint32_t seq_num);

private:
	void generate_impl();
};


class LongUplinkScrambler : public Scrambler< LongUplinkScrambler >
{
friend class Scrambler< LongUplinkScrambler >;
public:
	LongUplinkScrambler() : Scrambler< LongUplinkScrambler >() { }

	LongUplinkScrambler(uint32_t seq_num, uint32_t offset);

private:
	void generate_impl();

	uint32_t offset_;
};

#endif // SCRAMBLE_HPP
