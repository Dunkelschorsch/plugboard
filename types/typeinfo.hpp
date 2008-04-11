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

#ifndef TYPEINFO_HPP
#define TYPEINFO_HPP

#include "types/base.hpp"

namespace plugboard
{
	// declare neccessary signal classes
	#define BOOST_PP_DEF(z, I, _) \
		class SIG_TYPE(I); \

	BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

	#undef BOOST_PP_DEF

	template< typename T >
	struct typeinfo;

	#define BOOST_PP_DEF(z, I, _) \
		template< > \
		struct typeinfo< CPP_TYPE(I) > \
		{	\
			typedef SIG_TYPE(I) signal_type; \
			typedef VEC_TYPE(I) vector_type; \
			static const type_t value = TYPE_VALUE(I); \
			static const size_t size = sizeof(CPP_TYPE(I)); \
		};

	BOOST_PP_REPEAT(SIGNAL_TYPE_CNT, BOOST_PP_DEF, _)

	#undef BOOST_PP_DEF
} // namespace plugboard

#endif // TYPEINFO_HPP
