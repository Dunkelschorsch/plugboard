# ----------------------------------------------------------------------------
#
# PlugBoard - A versatile communication simulation framework
# Copyright (C) 2007-2008  Armin Schmidt
#
# This file is part of PlugBoard.
#
# PlugBoard is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# PlugBoard is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with PlugBoard.  If not, see <http://www.gnu.org/licenses/>.
#
# ----------------------------------------------------------------------------


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
	RaisedCosineReceive
	RaisedCosineTransmit
	RandomUniformBitGenerator
	StdoutSink
)
	add_library (block_${blockname} SHARED block_${blockname}.cpp)
	target_link_libraries (block_${blockname} block_common ${ITPP})

endforeach (blockname)

add_subdirectory(scrambler)
