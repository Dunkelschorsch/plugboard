project (HUMP)

add_executable (main main.cpp symtab.cpp variable.cpp system.cpp block_loader.cpp block.cpp port.cpp)

include_directories (${HUMP_SOURCE_DIR})

# that would work in case of system-wide installed libs

find_package(Boost)
if (NOT Boost_FOUND)
	message(error 'Cannot Find Boost!)
endif (NOT Boost_FOUND)

include_directories (${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})

#find and link against the boost::fileysytem library
find_library(BOOST_FILESYSTEM boost_filesystem)
if(NOT BOOST_FILESYSTEM)
	message(error 'Could not find the boost::filesystem library!')
endif(NOT BOOST_FILESYSTEM)

target_link_libraries(main ${BOOST_FILESYSTEM} ltdl)

#find and link against the boost::signals library
#find_library(BOOST_SIGNALS boost_signals)
#if(NOT BOOST_SIGNALS)
#	message(error 'Could not find the boost::signals library!')
#endif(NOT BOOST_SIGNALS)

#target_link_libraries(main ${BOOST_SIGNALS})

add_library (block_common SHARED block.cpp)

add_subdirectory (blocks)
