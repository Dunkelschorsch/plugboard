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

#ifndef VISIBILITY_HPP
#define VISIBILITY_HPP


// #ifdef _MSC_VER
//   #ifdef BUILDING_DLL
//     #define DSOEXPORT __declspec(dllexport)
//   #else
//     #define DSOEXPORT __declspec(dllimport)
//   #endif
//   #define DLLLOCAL
// #else
//   #ifdef HAVE_GCCVISIBILITYPATCH
//     #define DSOEXPORT __attribute__ ((visibility("default")))
//     #define DLLLOCAL __attribute__ ((visibility("hidden")))
//   #else
//     #define DSOEXPORT
//     #define DLLLOCAL
//   #endif
// #endif


#ifdef DISABLE_VISIBILITY_ALTOGETHER
    #define DSOEXPORT
    #define DSOLOCAL
#else
    #define DSOEXPORT __attribute__ ((visibility("default")))
    #define DSOLOCAL __attribute__ ((visibility("hidden")))
#endif

#endif // VISIBILITY_HPP
