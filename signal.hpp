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

#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include "types/base.hpp"

#include <vector>

namespace plugboard
{
	class Signal
	{
	public:
		typedef std::vector< Signal* > store_t;

		virtual ~Signal() {};

	protected:
		Signal() : signal_type_(empty), dimensions_(), Ts_(0) { }

		type_t signal_type_;

	private:
		std::vector< uint8_t > dimensions_;

		real_t Ts_;
	};



	template< class ElementT >
	class SignalStore
	{
	public:
		typedef ElementT element_t;

		SignalStore(size_t size) :
			size_(size), data_(size)
		{
			data_.zeros();
		}

		SignalStore(const SignalStore& other) :
			size_(other.size_), data_(other.data_)
		{ }

		itpp::Vec< ElementT >* get_data()
		{
			return &data_;
		}

	private:
		size_t size_;
		itpp::Vec< ElementT > data_;
	};

// this macro creates all necessary Signal-derived classes
#define BOOST_PP_DEF(z, I, _)  \
	class SIG_TYPE(I) : public Signal, public SignalStore< CPP_TYPE(I) > \
	{						\
		public:					\
		SIG_TYPE(I)(size_t size) : SignalStore< CPP_TYPE(I) >(size)\
		{					\
	 		signal_type_ = TYPE_VALUE(I);	\
		};					\
	};

BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)
#undef BOOST_PP_DEF

} // namespace plugboard

#endif // SIGNAL_HPP
