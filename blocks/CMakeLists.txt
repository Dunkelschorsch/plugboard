foreach (blockname dummy2 dummy3 stdout_sink 2in2out 2in1out 1in2out 1out 1in 1in1out)
	add_library (block_${blockname} SHARED block_${blockname}.cpp)
	target_link_libraries (block_${blockname} block_common)
endforeach (blockname)
