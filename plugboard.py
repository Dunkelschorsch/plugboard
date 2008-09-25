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


import libplugboard
import numpy

libplugboard.load_blocks("blocks")

def add_block(block_type, block_name, *block_arguments):
	
	arguments = list()
	for i in block_arguments:
		arguments.append(numpy.atleast_1d(i))

	libplugboard.add_block(block_type, block_name, tuple(arguments))

	return

def connect(source_block_name, source_port_name, sink_block_name, sink_port_name):
	libplugboard.connect(source_block_name, source_port_name, sink_block_name, sink_port_name)
	return


def run(times):
	libplugboard.run(times)
	return

