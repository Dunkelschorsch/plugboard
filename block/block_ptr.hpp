#ifndef BLOCK_BLOCK_PTR_HPP
#define BLOCK_BLOCK_PTR_HPP

#include <boost/intrusive_ptr.hpp>
#include "visibility.hpp"

namespace plugboard
{
	class Block;
	class Sink;
	class Source;

	typedef boost::intrusive_ptr< Block > block_ptr;
	typedef boost::intrusive_ptr< Sink > sink_ptr;
	typedef boost::intrusive_ptr< Source > source_ptr;

	typedef boost::intrusive_ptr< const Block > const_block_ptr;
	typedef boost::intrusive_ptr< const Sink > const_sink_ptr;
	typedef boost::intrusive_ptr< const Source > const_source_ptr;


	int intrusive_ptr_release(Block*) DSOEXPORT;
	int intrusive_ptr_add_ref(Block*) DSOEXPORT;

	int intrusive_ptr_release(const Source*) DSOEXPORT;
	int intrusive_ptr_add_ref(const Source*) DSOEXPORT;

	int intrusive_ptr_release(const Sink*) DSOEXPORT;
	int intrusive_ptr_add_ref(const Sink*) DSOEXPORT;
}

#endif // BLOCK_BLOCK_PTR_HPP
