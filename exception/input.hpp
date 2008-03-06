#ifndef EXCEPTION_INPUT_HPP
#define EXCEPTION_INPUT_HPP

#include "exception/base.hpp"


// this one is thrown from the block library, hence the export
class DSOEXPORT IncompatibleTypesException : public HumpException< std::string >
{
public:
	IncompatibleTypesException(const std::string& unknownId);

	const char* do_what() const;
};



class ParameterCountMismatchException : public HumpException< std::string >
{
public:
	ParameterCountMismatchException(const std::string& unknownId);

	const char* do_what() const;
};



class UndefinedVariableException : public HumpException< std::string >
{
public:
	UndefinedVariableException(const std::string& unknownId);

	const char* do_what() const;
};

#endif // EXCEPTION_INPUT_HPP
