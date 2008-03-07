#include "scramble.hpp"

#ifndef NDEBUG
#include <iostream>
#endif

DownlinkScrambler::DownlinkScrambler(uint32_t seq_num) :
	Scrambler< DownlinkScrambler >(38400, seq_num)
{ }


void DownlinkScrambler::generate_impl()
{
	uint32_t registerX;
	uint32_t tmpRegisterX;
	uint8_t  tmpNewX;
	uint32_t registerY;
	uint32_t tmpRegisterY;
	uint8_t  tmpNewY;
	uint8_t  tmpNewC_dl_i, tmpNewC_dl_q;
	itpp::Vec< short int > mapping("+1 -1");

	registerX = 0x01;
	registerY = 0x3ffff;

	for (uint32_t i=0; i<seq_num_; i++)
	{
		tmpRegisterX = registerX;
		// compute feedback term from register X
		tmpNewX = LSB (tmpRegisterX);
		tmpRegisterX >>= 7;
		tmpNewX ^= LSB (tmpRegisterX);

		registerX >>= 1;
		registerX |= (tmpNewX << 17);
	}

	for (uint32_t i=0; i<seq_len_; i++)
	{
		tmpRegisterX = registerX;
		tmpRegisterY = registerY;
#ifndef NDEBUG
		// for verifying period lengths of the pn (and hopefully M-) sequences
		if ( registerX == 0x01 )
			std::cout << "LSFR initial state registerX: "
				<< registerX << ": " << i << std::endl;
		if ( registerY == 0x3ffff )
			std::cout << "LSFR initial state registerY: "
				<< registerY << ": " << i << std::endl;
#endif
		tmpNewC_dl_i = LSB (registerX) ^ LSB (registerY);

		tmpNewX = LSB (tmpRegisterX);
		tmpRegisterX >>= 4;
		tmpNewC_dl_q   = LSB (tmpRegisterX);
		tmpRegisterX >>= 2;
		tmpNewC_dl_q  ^= LSB (tmpRegisterX);
		tmpRegisterX >>= 1;
		tmpNewX ^= LSB (tmpRegisterX);
		tmpRegisterX >>= 8;
		tmpNewC_dl_q  ^= LSB (tmpRegisterX);

		registerX >>= 1;
		registerX |= (tmpNewX << 17);

		// compute feedback and quadrature term from register Y
		tmpNewY = LSB (tmpRegisterY);
		tmpRegisterY >>= 5;
		tmpNewY ^= LSB (tmpRegisterY);

		tmpNewC_dl_q ^= LSB (tmpRegisterY);
		tmpRegisterY >>= 1;
		tmpNewC_dl_q ^= LSB (tmpRegisterY);
		tmpRegisterY >>= 1;
		tmpNewY ^= LSB (tmpRegisterY);
		tmpRegisterY >>= 1;
		tmpNewC_dl_q ^= LSB (tmpRegisterY);
		tmpRegisterY >>= 1;
		tmpNewC_dl_q ^= LSB (tmpRegisterY);
		tmpRegisterY >>= 1;
		tmpNewY ^= LSB (tmpRegisterY);
		tmpNewC_dl_q ^= LSB (tmpRegisterY);
		tmpRegisterY >>= 1;
		tmpNewC_dl_q ^= LSB (tmpRegisterY);
		tmpRegisterY >>= 1;
		tmpNewC_dl_q ^= LSB (tmpRegisterY);
		tmpRegisterY >>= 1;
		tmpNewC_dl_q ^= LSB (tmpRegisterY);
		tmpRegisterY >>= 1;
		tmpNewC_dl_q ^= LSB (tmpRegisterY);
		tmpRegisterY >>= 1;
		tmpNewC_dl_q ^= LSB (tmpRegisterY);

		registerY >>= 1;
		registerY |= (tmpNewY << 17);

		c_n_real_(i) = mapping(tmpNewC_dl_i);
		c_n_imag_(i) = mapping(tmpNewC_dl_q);
	} // for
	Z_ = itpp::to_cvec(c_n_real_, c_n_imag_);
} // void generate



ShortUplinkScrambler::ShortUplinkScrambler(uint32_t seq_num) :
	Scrambler< ShortUplinkScrambler >(255, seq_num)
{ }


void ShortUplinkScrambler::generate_impl()
{
	// initialize linear feedback shift registers
	uint16_t registerA;
	uint16_t tmpRegisterA;
	uint8_t tmpNewA;

	uint8_t registerB;
	uint8_t tmpRegisterB;
	uint8_t tmpNewB;

	uint8_t registerD;
	uint8_t tmpRegisterD;
	uint8_t tmpNewD;

	itpp::Vec< uint8_t > z(seq_len_);
	itpp::Mat< short int > mapping = ("+1 -1 -1 +1 ; +1 +1 -1 -1");

	registerA = 0;
	//  0000 0000 abcd efgh  ->  a0b0 c0d0 e0f0 g0h1
	uint32_t i;
	for (i=0; i<8; i++)
		registerA |= ((seq_num_ >> (7-i)) & 0x01) << (15-(2*i));
	registerA++;

	// the sequence number is 24 bits wide
	seq_num_ >>= 8;
	registerB = seq_num_ & 0xff;   //	0000 0000 -> 1111 1111 <- 0000 0000
	seq_num_ >>= 8;
	registerD = seq_num_ & 0xff;   // -> 1111 1111 <- 0000 0000	0000 0000

	for (i=0; i<seq_len_; i++)
	{
		tmpRegisterA = registerA;
		tmpRegisterB = registerB;
		tmpRegisterD = registerD;

		z(i) = ( LnSB<2>(registerA) + 2*LSB (registerB) + 2*LSB (registerD) ) % 4;

		c_1_n_(i) = mapping(0, z(i));
		c_2_n_(i) = mapping(0, z(i));

		c_n_real_(i) = c_1_n_(i);

		if (i % 2 == 0)
			c_n_imag_(i) = c_1_n_(i) * c_2_n_(i);
		else
			c_n_imag_(i) = (-1) * c_1_n_(i) * c_2_n_(i-1);

		// compute sequence a(i) using the linear feedback shift register
		tmpNewA  = 3 * LnSB<2>(tmpRegisterA); // 0000 0000 0000 0011
		tmpRegisterA >>= 2;
		tmpNewA += 2 * LnSB<2>(tmpRegisterA);
		tmpRegisterA >>= 2;
		tmpNewA += 3 * LnSB<2>(tmpRegisterA);
		tmpRegisterA >>= 2;
		tmpNewA += 1 * LnSB<2>(tmpRegisterA);
		tmpRegisterA >>= 4;
		tmpNewA += 3 * LnSB<2>(tmpRegisterA);

		tmpNewA %= 4;

		registerA >>= 2;
		registerA |= (tmpNewA << 14);

		// compute linear feedback shift register for sequence b(i)
		tmpNewB =  LSB (tmpRegisterB);
		tmpRegisterB >>= 1;
		tmpNewB ^= LSB (tmpRegisterB);
		tmpRegisterB >>= 4;
		tmpNewB ^= LSB (tmpRegisterB);
		tmpRegisterB >>= 2;
		tmpNewB ^= LSB (tmpRegisterB);

		registerB >>= 1;
		registerB |= (tmpNewB << 7);

		// compute linear feedback shift register for sequence d(i)
		tmpNewD =  LSB (tmpRegisterD);
		tmpRegisterD >>= 4;
		tmpNewD ^= LSB (tmpRegisterD);
		tmpRegisterD >>= 1;
		tmpNewD ^= LSB (tmpRegisterD);
		tmpRegisterD >>= 2;
		tmpNewD ^= LSB (tmpRegisterD);

		registerD >>= 1;
		registerD |= (tmpNewD << 7);
	}
	Z_ = to_cvec(c_n_real_, c_n_imag_);
}



LongUplinkScrambler::LongUplinkScrambler(uint32_t seq_num, uint32_t offset) :
	Scrambler< LongUplinkScrambler >(38400, seq_num),
	offset_(offset)
{ }


void LongUplinkScrambler::generate_impl()
{
	itpp::Vec< short int > mapping("+1 -1");

	// define linear feedback shift registers
	uint32_t registerA, tmpRegisterA;
	uint32_t registerB, tmpRegisterB;
	uint8_t tmpNewA, tmpNewB;

	uint8_t tmpNewC_long_1_n, tmpNewC_long_2_n;

	/*
	 * Both registers are 25 bit wide, not 24 bit! I repeat: 25.
	 * Written out: TWENTY FIVE !!!!!!
	 */

	// mask: 0000 0001 0000 0000 0000 0000 0000 0000
	registerA = ( seq_num_ & 0xffffff ) | 0x1000000;

	// value: 0000 0001 1111 1111 1111 1111 1111 1111
	registerB = 0x1ffffff;

	for (uint32_t i=0; i < seq_len_+offset_; i++)
	{
		tmpRegisterA = registerA;
		tmpRegisterB = registerB;
#ifndef NDEBUG
		// both LFSRs have maximum period length 2^25-1
		if ( registerA ==  (seq_num_ | 0x1000000) )
			std::cout << "LSFR initial state registerA: "
				<< registerA << ": " << i << std::endl;
		if ( registerB == 0x1ffffff )
			std::cout << "LSFR initial state registerB: "
				<< registerB << ": " << i << std::endl;
#endif
		tmpNewC_long_1_n = LSB (registerA) ^ LSB (registerB);

		// compute feedback bit for register A
		tmpNewA = LSB (tmpRegisterA); // 0 0000 0000 0000 0000 0000 0001
		tmpRegisterA >>= 3;
		tmpNewA ^= LSB (tmpRegisterA);

		// compute c_2_n_ (from register A)
		tmpRegisterA >>= 1;
		tmpNewC_long_2_n = LSB (tmpRegisterA);
		tmpRegisterA >>= 3;
		tmpNewC_long_2_n ^= LSB (tmpRegisterA);
		tmpRegisterA >>= 11;
		tmpNewC_long_2_n ^= LSB (tmpRegisterA);

		registerA >>= 1;
		registerA |= (tmpNewA << 24);

		// compute feedback bit
		tmpNewB  = LSB (tmpRegisterB);
		tmpRegisterB >>= 1;
		tmpNewB ^= LSB (tmpRegisterB);
		tmpRegisterB >>= 1;
		tmpNewB ^= LSB (tmpRegisterB);
		tmpRegisterB >>= 1;
		tmpNewB ^= LSB (tmpRegisterB);

		// continue computing c_2_n_ (from register B)
		tmpRegisterB >>= 1;
		tmpNewC_long_2_n ^= LSB (tmpRegisterB);
		tmpRegisterB >>= 2;
		tmpNewC_long_2_n ^= LSB (tmpRegisterB);
		tmpRegisterB >>= 11;
		tmpNewC_long_2_n ^= LSB (tmpRegisterB);

		registerB >>= 1;
		registerB |= (tmpNewB << 24);

		if(i >= offset_)
		{
			uint32_t idx = i - offset_;
		
			c_1_n_(idx) = mapping(tmpNewC_long_1_n);
			c_2_n_(idx) = mapping(tmpNewC_long_2_n);
		
			// c_1_n_ is just the real part of the scrambling sequence
			c_n_real_(idx) = mapping(tmpNewC_long_1_n);
		
			if((i % 2) == 0) // even 'i's
				c_n_imag_(idx) = c_1_n_(idx) * c_2_n_(idx);
			else // odd 'i's
				c_n_imag_(idx) = (-1) * c_1_n_(idx) * c_2_n_(idx-1);
		}
	}
	Z_ = to_cvec(c_n_real_, c_n_imag_);
}
