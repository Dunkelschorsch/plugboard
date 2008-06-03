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

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

SET(CMAKE_CC_FLAGS "-Wall -pipe -fvisibility=hidden" CACHE STRING "This is appended to the C++ flags of each mode.")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CC_FLAGS}")

SET(CMAKE_CC_RELEASE "-O3 -fomit-frame-pointer -DNDEBUG -s" CACHE STRING "This is appended to the C++ flags in release mode.")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CC_RELEASE}")

SET(CMAKE_CC_FLAGS_DEBUG "-ggdb -Werror" CACHE STRING "This is appended to the C++ flags in debug mode.")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CC_FLAGS_DEBUG}")

INCLUDE_DIRECTORIES(${PLUGBOARD_SOURCE_DIR})

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

FIND_PACKAGE(Boost)
IF(NOT Boost_FOUND)
	MESSAGE(FATAL_ERROR "Cannot find Boost!")
ELSE(NOT Boost_FOUND)
	MESSAGE(STATUS "Boost include directory: ${Boost_INCLUDE_DIRS}")
	MESSAGE(STATUS "Boost library directory: ${Boost_LIBRARY_DIRS}")
ENDIF(NOT Boost_FOUND)

# find and link against the boost::fileysytem library
FIND_LIBRARY(BOOST_FILESYSTEM boost_filesystem)
IF(NOT BOOST_FILESYSTEM)
	MESSAGE(FATAL_ERROR "Could not find the Boost filesystem library!")
ELSE(NOT BOOST_FILESYSTEM)
	MESSAGE(STATUS "Boost filesystem library: ${BOOST_FILESYSTEM}")
ENDIF(NOT BOOST_FILESYSTEM)

# find and link against the boost::thread library
FIND_LIBRARY(BOOST_THREAD boost_thread)
IF(NOT BOOST_THREAD)
	MESSAGE(FATAL_ERROR "Could not find the Boost thread library!")
ELSE(NOT BOOST_THREAD)
	MESSAGE(STATUS "Boost thread library: ${BOOST_THREAD}")
ENDIF(NOT BOOST_THREAD)

# find and link against the boost::program_options library
FIND_LIBRARY(BOOST_PROGRAM_OPTIONS boost_program_options)
IF(NOT BOOST_PROGRAM_OPTIONS)
	MESSAGE(FATAL_ERROR "Could not find the Boost program_options library!")
ELSE(NOT BOOST_PROGRAM_OPTIONS)
	MESSAGE(STATUS "Boost program_options library: ${BOOST_PROGRAM_OPTIONS}")
ENDIF(NOT BOOST_PROGRAM_OPTIONS)

# find and link against the boost::python library
FIND_LIBRARY(BOOST_PYTHON boost_python)
IF(NOT BOOST_PYTHON)
	MESSAGE(FATAL_ERROR "Could not find the Boost Python library!")
ELSE(NOT BOOST_PYTHON)
	MESSAGE(STATUS "Boost python library: ${BOOST_PYTHON}")
ENDIF(NOT BOOST_PYTHON)

# find and link against the ltdl library
FIND_LIBRARY(LTDL ltdl)
IF(NOT LTDL)
	MESSAGE(FATAL_ERROR "Could not find the ltdl library!")
ELSE(NOT LTDL)
	MESSAGE(STATUS "ltdl library: ${LTDL}")
ENDIF(NOT LTDL)

# find and link against the itpp library
FIND_LIBRARY(ITPP NAME itpp PATHS ~/stuff64/lib)
IF(NOT LTDL)
	MESSAGE(FATAL_ERROR "Could not find ITPP library!")
ELSE(NOT LTDL)
	MESSAGE(STATUS "itpp library: ${ITPP}")
ENDIF(NOT LTDL)


FIND_PACKAGE(PythonLibs)
IF(NOT PYTHON_INCLUDE_PATH)
	MESSAGE(FATAL_ERROR "Could not find the Python libraries")
ELSE(NOT PYTHON_INCLUDE_PATH)

	FIND_FILE(PYCONFIG_H pyconfig.h ${PYTHON_INCLUDE_PATH})
	IF(NOT PYCONFIG_H)
		MESSAGE(FATAL_ERROR "Could not find ${PYTHON_INCLUDE_PATH}/pyconfig.h")
	ENDIF(NOT PYCONFIG_H)

	FIND_FILE(PYTHON_H pyconfig.h ${PYTHON_INCLUDE_PATH})
	IF(NOT PYTHON_H)
		MESSAGE(FATAL_ERROR "Could not find ${PYTHON_INCLUDE_PATH}/Python.h")
	ENDIF(NOT PYTHON_H)

ENDIF(NOT PYTHON_INCLUDE_PATH)


SET(CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIRS})
# this is not nice. on the other hand, not having the library installed system-wide isn't nice either...
INCLUDE_DIRECTORIES(
	~/stuff64/include
	${Boost_INCLUDE_DIRS}
	${PYTHON_INCLUDE_PATH}
)

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

TARGET_LINK_LIBRARIES(pb
	${BOOST_FILESYSTEM}
	${BOOST_THREAD}
	${BOOST_PROGRAM_OPTIONS}
	${ITPP}
	${LTDL}
	block_common
)

ADD_LIBRARY(plugboard SHARED
	plugboard_py.cpp
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

TARGET_LINK_LIBRARIES(plugboard
	${BOOST_FILESYSTEM}
	${BOOST_THREAD}
	${BOOST_PYTHON}
	${ITPP}
	${LTDL}
	block_common
)

ADD_LIBRARY(block_common SHARED
	block/block.cpp
	block/source.cpp
	block/sink.cpp
	block/block_ptr.cpp
	parameter.cpp
	port/port_in.cpp
	port/port_out.cpp
)

ADD_SUBDIRECTORY(blocks)
