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


#ifdef DEBUG_CORE

#ifndef PB_DEBUG_MESSAGE_SOURCE
#define PB_DEBUG_MESSAGE_SOURCE Debug
#endif

#ifndef PB_DEBUG_MESSAGE_COLOUR
#define PB_DEBUG_MESSAGE_COLOUR \033[01;37m
#endif

#include <boost/preprocessor/stringize.hpp>
#include <iostream>

#define PB_DEBUG_MESSAGE_IMPL(C,S,X) { std::cout << C << "[" << S << "] " << X << "\033[01;37m" << std::endl; }
#define PB_DEBUG_MESSAGE_LOCKED_IMPL(C,S,X)\
{\
	boost::mutex::scoped_lock lock(pb_io_mutex);\
	PB_DEBUG_MESSAGE_IMPL(C,S,X)\
}

#define PB_DEBUG_MESSAGE(X) PB_DEBUG_MESSAGE_IMPL(BOOST_PP_STRINGIZE(PB_DEBUG_MESSAGE_COLOUR), BOOST_PP_STRINGIZE(PB_DEBUG_MESSAGE_SOURCE), X)
#define PB_DEBUG_MESSAGE_LOCKED(X) PB_DEBUG_MESSAGE_LOCKED_IMPL(BOOST_PP_STRINGIZE(PB_DEBUG_MESSAGE_COLOUR), BOOST_PP_STRINGIZE(PB_DEBUG_MESSAGE_SOURCE), X)

#endif
