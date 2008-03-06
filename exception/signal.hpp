#ifndef EXCEPTION_SIGNAL_HPP
#define EXCEPTION_SIGNAL_HPP

#include "exception/base.hpp"


class SampleTimesMismatchException : public HumpException< std::string >
{
public:
	SampleTimesMismatchException(const std::string& unknownId);

	const char* do_what() const;
};



class SignalTypesMismatchException : public HumpException< std::string >
{
public:
	SignalTypesMismatchException(const std::string& unknownId);

	const char* do_what() const;
};



class FrameSizesMismatchException : public HumpException< std::string >
{
public:
	FrameSizesMismatchException(const std::string& unknownId);

	const char* do_what() const;
};


#endif // EXCEPTION_SIGNAL_HPP
