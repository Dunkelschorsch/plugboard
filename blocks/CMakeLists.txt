# FIND_LIBRARY(ITPP NAME itpp PATHS ~/stuff64/lib)

foreach(
	blockname
	Arithmetic
	AWGNChannel
	BitErrorCount
	ComposeComplex
	ConvolutionalDecoder
	ConvolutionalEncoder
	Fork
	MPSKDemodulator
	MPSKModulator
	RandomUniformBitGenerator
	StdoutSink
)
	add_library (block_${blockname} SHARED block_${blockname}.cpp)
	target_link_libraries (block_${blockname} block_common ${ITPP})

endforeach (blockname)


SET(blockname "Scrambler")
add_library (block_${blockname} SHARED block_${blockname}/block_${blockname}.cpp block_${blockname}/scramble.cpp)
target_link_libraries (block_${blockname} block_common ${ITPP})

