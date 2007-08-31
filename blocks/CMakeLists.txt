foreach (blockname dummy2 dummy3 stdout_sink)
	add_library (block_${blockname} SHARED block_${blockname}.cpp)
	target_link_libraries (block_${blockname} block_common)
endforeach (blockname)
