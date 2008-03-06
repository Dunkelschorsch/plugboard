#ifndef EXCEPTION_BLOCK_HPP
#define EXCEPTION_BLOCK_HPP

#include "exception/base.hpp"


class InvalidBlockNameException : public HumpException< std::string >
{
public:
	InvalidBlockNameException(const std::string& unknownId);

	const char* do_what() const;
};


class InvalidBlockIdException : public HumpException< std::string >
{
public:
	InvalidBlockIdException(const std::string& unknownId);

	const char* do_what() const;
};


class DuplicateBlockNameException : public HumpException< std::string >
{
public:
	DuplicateBlockNameException(const std::string& unknownId);

	const char* do_what() const;
};


// unused atm
class BlockNotConfiguredException : public HumpException< std::string >
{
public:
	BlockNotConfiguredException(const std::string& unknownId);

	const char* do_what() const;
};

#endif // EXCEPTION_BLOCK_HPP
