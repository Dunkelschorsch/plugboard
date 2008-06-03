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

