# FIND_LIBRARY(ITPP NAME itpp PATHS ~/stuff64/lib)



foreach(
	blockname
	stdout_sink
	2in2out
	2in1out
	1in2out
	1out
	1in
	1in1out
	RandomUniformBitGenerator
	MPSKModulator
	MPSKDemodulator
	BitErrorCount
	AWGNChannel
	ConvolutionalEncoder
	ConvolutionalDecoder
)
	add_library (block_${blockname} SHARED block_${blockname}.cpp)
	target_link_libraries (block_${blockname} block_common ${ITPP})

endforeach (blockname)
