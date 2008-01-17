project (HUMP)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

include_directories (${HUMP_SOURCE_DIR} ${HUMP_SOURCE_DIR}/swift ${HUMP_SOURCE_DIR}/input)

add_executable(
	main
	main.cpp
	symtab.cpp
	variable.cpp
	system.cpp
	systems.cpp
	subsystem.cpp
	exec_matrix.cpp
	exec_stage.cpp
	block_loader.cpp
	parameter.cpp
# 	block.cpp
	port_in.cpp
	port_out.cpp
# 	input/console.cpp
	input/actions.cpp
	input/file.cpp
)



find_package(Boost)
if (NOT Boost_FOUND)
	message(fatal_error 'Cannot find Boost!)
endif (NOT Boost_FOUND)


#find_package(Readline)
#if (NOT READLINE_FOUND)
#	message(fatal_error 'Cannot find Readline!')
#endif (NOT READLINE_FOUND)


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


include_directories (${Boost_INCLUDE_DIRS} ${READLINE_INCLUDE_DIR})
link_directories (${Boost_LIBRARY_DIRS})


SET(CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIRS} ${READLINE_INCLUDE_DIR})

target_link_libraries(main ${BOOST_FILESYSTEM} ${BOOST_THREAD} ${READLINE_LIBRARY} block_common ltdl)

add_library (block_common SHARED block.cpp)

add_subdirectory (blocks)
