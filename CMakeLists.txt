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

project (PLUGBOARD)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

SET(CMAKE_CC_FLAGS "-Wall -ansi -pipe -fvisibility=hidden" CACHE STRING "This is appended to the C++ flags of each mode.")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CC_FLAGS}")

SET(CMAKE_CC_RELEASE "-O3 -fomit-frame-pointer -DNDEBUG -s" CACHE STRING "This is appended to the C++ flags in release mode.")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CC_RELEASE}")

SET(CMAKE_CC_FLAGS_DEBUG "-ggdb -Werror")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CC_FLAGS_DEBUG}")

include_directories (${PLUGBOARD_SOURCE_DIR})

add_executable(
	pb
	main.cpp
	environment.cpp
	symtab.cpp
	variable/variable.cpp
	system.cpp
	systems.cpp
	subsystem.cpp
	exec_matrix.cpp
	exec_stage.cpp
	exception/exceptions.cpp
	block_loader.cpp
	parameter.cpp
	port/port_in.cpp
	port/port_out.cpp
	grammar/command/actions.cpp
	input/file.cpp
)


find_package(Boost)
if (NOT Boost_FOUND)
	message(FATAL_ERROR "Cannot find Boost!")
endif (NOT Boost_FOUND)


# find and link against the boost::fileysytem library
find_library(BOOST_FILESYSTEM boost_filesystem)
if(NOT BOOST_FILESYSTEM)
	message(FATAL_ERROR "Could not find the boost::filesystem library!")
endif(NOT BOOST_FILESYSTEM)

# find and link against the boost::thread library
find_library(BOOST_THREAD boost_thread)
if(NOT BOOST_THREAD)
	message(FATAL_ERROR "Could not find the boost::thread library!")
endif(NOT BOOST_THREAD)

# find and link against the boost::thread library
find_library(BOOST_PROGRAM_OPTIONS boost_program_options)
if(NOT BOOST_PROGRAM_OPTIONS)
	message(FATAL_ERROR "Could not find the boost::program_options library!")
endif(NOT BOOST_PROGRAM_OPTIONS)

# find and link against the ltdl library
find_library(LTDL ltdl)
if(NOT LTDL)
	MESSAGE(FATAL_ERROR "Could not find the ltdl library!")
endif(NOT LTDL)


FIND_LIBRARY(ITPP NAME itpp PATHS ~/stuff64/lib)

# this is not nice. on the other hand, not having the library installed system-wide isn't nice either...
include_directories (~/stuff64/include)

include_directories (${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})


SET(CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIRS})

target_link_libraries(pb ${BOOST_FILESYSTEM} ${BOOST_THREAD} ${BOOST_PROGRAM_OPTIONS} ${ITPP} ${LTDL} block_common)

add_library (block_common SHARED block/block.cpp block/source.cpp block/sink.cpp)

add_subdirectory (blocks)
