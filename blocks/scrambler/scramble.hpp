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

protected:
	Scrambler() : seq_num_(0), seq_len_(0) { }

	Scrambler(uint32_t seq_len, uint32_t seq_num) :
		seq_num_(seq_num),
		seq_len_(seq_len),
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

	uint32_t seq_num_, seq_len_;
	
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
