#ifndef EXCEPTION_BLOCK_HPP
#define EXCEPTION_BLOCK_HPP

#include "exception/base.hpp"

namespace plugboard
{
	class InvalidBlockNameException : public Exception< std::string >
	{
	public:
		InvalidBlockNameException(const std::string& unknownId);

		const char* do_what() const;
	};


	class InvalidBlockIdException : public Exception< std::string >
	{
	public:
		InvalidBlockIdException(const std::string& unknownId);

		const char* do_what() const;
	};


	class DuplicateBlockNameException : public Exception< std::string >
	{
	public:
		DuplicateBlockNameException(const std::string& unknownId);

		const char* do_what() const;
	};


	// unused atm
	class BlockNotConfiguredException : public Exception< std::string >
	{
	public:
		BlockNotConfiguredException(const std::string& unknownId);

		const char* do_what() const;
	};
} // namespace plugboard

#endif // EXCEPTION_BLOCK_HPP
