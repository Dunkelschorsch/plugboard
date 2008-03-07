# add_library (scramble_common STATIC scramble.cpp)

foreach(
	blockname
	UMTSLongUplinkScrambler
	UMTSShortUplinkScrambler
	UMTSDownlinkScrambler
)
	add_library (block_${blockname} SHARED block_${blockname}.cpp scramble.cpp)
	target_link_libraries (block_${blockname} block_common ${ITPP})

endforeach (blockname)