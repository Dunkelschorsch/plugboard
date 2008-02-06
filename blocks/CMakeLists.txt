FIND_LIBRARY(ITPP NAME itpp PATHS ~/stuff64/lib)

# this is not nice. on the other hand, not having the library installed system-wide isn't nice either...
include_directories (~/stuff64/include)

foreach(
	blockname
	stdout_sink
	2in2out
	2in1out
	1in2out
	1out
	1in
	1in1out
)
	add_library (block_${blockname} SHARED block_${blockname}.cpp)
	target_link_libraries (block_${blockname} block_common ${ITPP})

endforeach (blockname)
