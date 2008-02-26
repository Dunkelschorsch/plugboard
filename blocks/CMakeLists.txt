# FIND_LIBRARY(ITPP NAME itpp PATHS ~/stuff64/lib)

foreach(
	blockname
	2in2out
	Fork
	1out
	stdoutSink
	RandomUniformBitGenerator
	MPSKModulator
	MPSKDemodulator
	BitErrorCount
	AWGNChannel
	ComposeComplex
	ConvolutionalEncoder
	ConvolutionalDecoder
)
	add_library (block_${blockname} SHARED block_${blockname}.cpp)
	target_link_libraries (block_${blockname} block_common ${ITPP})

endforeach (blockname)
