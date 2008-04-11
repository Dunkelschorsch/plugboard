#ifndef EXCEPTION_INPUT_HPP
#define EXCEPTION_INPUT_HPP

#include "exception/base.hpp"

namespace plugboard
{
	// this one is thrown from the block library, hence the export
	class DSOEXPORT IncompatibleTypesException : public Exception< std::string >
	{
	public:
		IncompatibleTypesException(const std::string& unknownId);

		const char* do_what() const;
	};



	class ParameterCountMismatchException : public Exception< std::string >
	{
	public:
		ParameterCountMismatchException(const std::string& unknownId);

		const char* do_what() const;
	};



	class UndefinedVariableException : public Exception< std::string >
	{
	public:
		UndefinedVariableException(const std::string& unknownId);

		const char* do_what() const;
	};
}

#endif // EXCEPTION_INPUT_HPP
