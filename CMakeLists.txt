project (HUMP)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

SET(CMAKE_CC_FLAGS "-Wall -ansi -pipe -fvisibility=hidden" CACHE STRING "Hallo")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CC_FLAGS}")

SET(CMAKE_CC_RELEASE "-O3 -fomit-frame-pointer -DNDEBUG -s" CACHE STRING "This is appended to the C++ flags in release mode.")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CC_RELEASE}")

SET(CMAKE_CC_FLAGS_DEBUG "-ggdb -Werror")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CC_FLAGS_DEBUG}")

include_directories (${HUMP_SOURCE_DIR})

add_executable(
	main
	main.cpp
	symtab.cpp
	variable/variable.cpp
	system.cpp
	systems.cpp
	subsystem.cpp
	exec_matrix.cpp
	exec_stage.cpp
	block_loader.cpp
	parameter.cpp
	port/port_in.cpp
	port/port_out.cpp
	grammar/variable/actions.cpp
	grammar/command/actions.cpp
	input/file.cpp
)


find_package(Boost)
if (NOT Boost_FOUND)
	message(FATAL_ERROR 'Cannot find Boost!)
endif (NOT Boost_FOUND)


# find and link against the boost::fileysytem library
find_library(BOOST_FILESYSTEM boost_filesystem)
if(NOT BOOST_FILESYSTEM)
	message(fatal_error 'Could not find the boost::filesystem library!')
endif(NOT BOOST_FILESYSTEM)


# find and link against the boost::thread library
find_library(BOOST_THREAD boost_thread)
if(NOT BOOST_THREAD)
	message(fatal_error 'Could not find the boost::thread library!')
endif(NOT BOOST_THREAD)

FIND_LIBRARY(ITPP NAME itpp PATHS ~/stuff64/lib)

# this is not nice. on the other hand, not having the library installed system-wide isn't nice either...
include_directories (~/stuff64/include)

include_directories (${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})


SET(CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIRS})

target_link_libraries(main ${BOOST_FILESYSTEM} ${BOOST_THREAD} ${ITPP} block_common ltdl)

add_library (block_common SHARED block/block.cpp block/source.cpp block/sink.cpp)

add_subdirectory (blocks)
