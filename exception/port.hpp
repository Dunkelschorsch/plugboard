#ifndef EXCEPTION_PORT_HPP
#define EXCEPTION_PORT_HPP

#include "exception/base.hpp"

namespace plugboard
{
	class InvalidPortNameException : public Exception< std::string >
	{
	public:
		InvalidPortNameException(const std::string& unknownId);

		const char* do_what() const;
	};


	class DuplicatePortNameException : public Exception< std::string >
	{
	public:
		DuplicatePortNameException(const std::string& unknownId);

		const char* do_what() const;
	};
}

#endif // EXCEPTION_PORT_HPP
