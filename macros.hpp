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

#ifndef MACROS_HPP
#define MACROS_HPP

#include <boost/shared_ptr.hpp>

#define PB_D(CLASS) typeof(d_func()) d = d_func();

#define PB_DECLARE_PRIVATE(CLASS)\
friend class CLASS##Impl;\
inline const boost::shared_ptr< CLASS##Impl > d_func() { return d_ptr; }\
inline const boost::shared_ptr< const CLASS##Impl > d_func() const { return d_ptr; }

#define PB_DECLARE_PROTECTED(CLASS)\
const boost::shared_ptr< CLASS##Impl >  d_ptr;\
CLASS(CLASS##Impl &dd);

#endif // MACROS_HPP
