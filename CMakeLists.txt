# ----------------------------------------------------------------------------
#
# PlugBoard - A versatile communication simulation framework
# Copyright (C) 2007-2008  Armin Schmidt
#
# This file is part of PlugBoard.
#
# PlugBoard is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# PlugBoard is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with PlugBoard.  If not, see <http://www.gnu.org/licenses/>.
#
# ----------------------------------------------------------------------------

PROJECT(PLUGBOARD)

cmake_minimum_required(VERSION 2.3)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

SET(CMAKE_CC_FLAGS "-Wall -pipe -march=native -fvisibility=hidden" CACHE STRING "This is appended to the C++ flags of each mode.")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CC_FLAGS}")

SET(CMAKE_CC_RELEASE "-O3 -fomit-frame-pointer -DNDEBUG -s" CACHE STRING "This is appended to the C++ flags in release mode.")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CC_RELEASE}")

SET(CMAKE_CC_FLAGS_DEBUG "-ggdb -Werror" CACHE STRING "This is appended to the C++ flags in debug mode.")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CC_FLAGS_DEBUG}")

INCLUDE_DIRECTORIES(${PLUGBOARD_SOURCE_DIR})


# find and link against the ltdl library
FIND_LIBRARY(LTDL ltdl)
IF(NOT LTDL)
	MESSAGE(FATAL_ERROR "Could not find the ltdl library.")
ELSE()
	MESSAGE(STATUS "ltdl library: ${LTDL}")
ENDIF()

# find and link against the itpp library
FIND_LIBRARY(ITPP NAME itpp PATHS ~/stuff64/lib)
IF(NOT ITPP)
	MESSAGE(FATAL_ERROR "Could not find ITPP library.")
ELSE()
	MESSAGE(STATUS "itpp library: ${ITPP}")
ENDIF()

FIND_PACKAGE(Boost)
IF(NOT Boost_FOUND)
	MESSAGE(FATAL_ERROR "Cannot find Boost.")
ELSE()
	MESSAGE(STATUS "Boost include directory: ${Boost_INCLUDE_DIRS}")
	MESSAGE(STATUS "Boost library directory: ${Boost_LIBRARY_DIRS}")
ENDIF()

# find and link against the boost::fileysytem library
FIND_LIBRARY(BOOST_FILESYSTEM boost_filesystem)
IF(NOT BOOST_FILESYSTEM)
	MESSAGE(FATAL_ERROR "Could not find the Boost filesystem library.")
ELSE()
	MESSAGE(STATUS "Boost filesystem library: ${BOOST_FILESYSTEM}")
ENDIF()

# find and link against the boost::thread library
FIND_LIBRARY(BOOST_THREAD boost_thread-mt)
IF(NOT BOOST_THREAD)
	MESSAGE(FATAL_ERROR "Could not find the Boost thread library.")
ELSE()
	MESSAGE(STATUS "Boost thread library: ${BOOST_THREAD}")
ENDIF()

# find and link against the boost::program_options library
FIND_LIBRARY(BOOST_PROGRAM_OPTIONS boost_program_options)
IF(NOT BOOST_PROGRAM_OPTIONS)
	MESSAGE(FATAL_ERROR "Could not find the Boost program_options library.")
ELSE()
	MESSAGE(STATUS "Boost program_options library: ${BOOST_PROGRAM_OPTIONS}")
ENDIF()




# only build python module if a suitable python and swig installation is found
SET(BUILD_PB_PYTHON TRUE)

FIND_PACKAGE(PythonLibs)
IF(NOT PYTHON_INCLUDE_PATH)
  MESSAGE(STATUS "Could not find the Python include directory.")
  SET(BUILD_PB_PYTHON FALSE)
ENDIF()

FIND_FILE(PYCONFIG_H pyconfig.h ${PYTHON_INCLUDE_PATH})
IF(NOT PYCONFIG_H)
  MESSAGE(STATUS "Could not find ${PYTHON_INCLUDE_PATH}/pyconfig.h.")
  SET(BUILD_PB_PYTHON FALSE)
ENDIF()

FIND_FILE(PYTHON_H Python.h ${PYTHON_INCLUDE_PATH})
IF(NOT PYTHON_H)
  MESSAGE(STATUS "Could not find ${PYTHON_INCLUDE_PATH}/Python.h.")
  SET(BUILD_PB_PYTHON FALSE)
ENDIF()

# look for swig
FIND_PACKAGE(SWIG)
  INCLUDE(${SWIG_USE_FILE})
IF(NOT SWIG_FOUND)
  MESSAGE(STATUS "Could not find SWIG.")
  SET(BUILD_PB_PYTHON FALSE)
ENDIF()

# look for numpy installation
GET_FILENAME_COMPONENT(NUMPY_INCLUDE_PATH ${PYTHON_LIBRARY} PATH)
SET(NUMPY_INCLUDE_PATH ${NUMPY_INCLUDE_PATH}/../site-packages/numpy/core/include/ CACHE STRING "location of the arrayobject.h file")

FIND_FILE(ARRAYOBJECT_H arrayobject.h ${NUMPY_INCLUDE_PATH}/numpy)
IF(NOT ARRAYOBJECT_H)
  MESSAGE(STATUS "Could not find numpy/arrayobject.h")
  SET(BUILD_PB_PYTHON FALSE)
ENDIF()

IF(BUILD_PB_PYTHON)
  MESSAGE(STATUS "Building plugboard Python module.")
ELSEIF()
  MESSAGE(STATUS "The plugboard Python module will NOT be built.")
ENDIF()


# this is not nice. on the other hand, not having the library installed system-wide isn't nice either...
INCLUDE_DIRECTORIES(
	~/stuff64/include
	${Boost_INCLUDE_DIRS}
)

SET(CMAKE_REQUIRED_INCLUDES
  ${Boost_INCLUDE_DIRS}
  ${NUMPY_INCLUDE_PATH}
)

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

ADD_LIBRARY(block_common SHARED
	block/block.cpp
	block/source.cpp
	block/sink.cpp
	block/block_ptr.cpp
	parameter.cpp
	port/port_in.cpp
	port/port_out.cpp
)

ADD_EXECUTABLE(pb
	main.cpp
	environment.cpp
	symtab.cpp
	variable/variable.cpp
	signal/signal_ptr.cpp
	system.cpp
	systems.cpp
	subsystem.cpp
	thread_mgm.cpp
	exec_matrix.cpp
	exec_stage.cpp
	exception/exceptions.cpp
	block_loader.cpp
	grammar/command/actions.cpp
	input/file.cpp
)

TARGET_LINK_LIBRARIES(pb
	${BOOST_FILESYSTEM}
	${BOOST_THREAD}
	${BOOST_PROGRAM_OPTIONS}
	${ITPP}
	${LTDL}
	block_common
)


# build plugboard python library
IF(BUILD_PB_PYTHON)
	INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${NUMPY_INCLUDE_PATH} ${PYTHON_INCLUDE_PATH} )
	SET_SOURCE_FILES_PROPERTIES(plugboard.i PROPERTIES CPLUSPLUS ON)

	SWIG_ADD_MODULE(plugboard python plugboard.i
		plugboard.cpp
		environment.cpp
		symtab.cpp
		variable/variable.cpp
		signal/signal_ptr.cpp
		system.cpp
		systems.cpp
		subsystem.cpp
		thread_mgm.cpp
		exec_matrix.cpp
		exec_stage.cpp
		exception/exceptions.cpp
		block_loader.cpp
	)
 
	SWIG_LINK_LIBRARIES(plugboard
		${BOOST_FILESYSTEM}
		${BOOST_THREAD}
		${ITPP}
		${LTDL}
		block_common
 	)
ENDIF()

ADD_SUBDIRECTORY(blocks)
