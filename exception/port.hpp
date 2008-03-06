#ifndef EXCEPTION_PORT_HPP
#define EXCEPTION_PORT_HPP

#include "exception/base.hpp"


class InvalidPortNameException : public HumpException< std::string >
{
public:
	InvalidPortNameException(const std::string& unknownId);

	const char* do_what() const;
};


class DuplicatePortNameException : public HumpException< std::string >
{
public:
	DuplicatePortNameException(const std::string& unknownId);

	const char* do_what() const;
};

#endif // EXCEPTION_PORT_HPP
