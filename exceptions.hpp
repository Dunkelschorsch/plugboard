#ifndef _EXCEPTIONS_HPP
#define _EXCEPTIONS_HPP

#include <stdexcept>


template < class IdentifierType >
class HumpException : public std::exception
{
public:
	HumpException(const IdentifierType& unknownId) : unknownId_(unknownId) { }

	const char* what()
	{
		return "Hump exception thrown!";
	}

	const IdentifierType get_id()
	{
		return unknownId_;
	};

	virtual ~HumpException() throw() { };
private:
	IdentifierType unknownId_;
};



class InvalidBlockNameException : public HumpException< std::string >
{
public:
	InvalidBlockNameException(const std::string& unknownId) : HumpException< std::string >(unknownId) { }

	const char* what()
	{
		return "Invalid block name passed";
	}
};



class InvalidBlockIdException : public HumpException< std::string >
{
public:
	InvalidBlockIdException(const std::string& unknownId) : HumpException< std::string >(unknownId) { }

	const char* what()
	{
		return "Invalid block ID passed";
	}
};



class InvalidPortNameException : public HumpException< std::string >
{
public:
	InvalidPortNameException(const std::string& unknownId) : HumpException< std::string >(unknownId) { }

	const char* what()
	{
		return "Invalid port name passed";
	}
};



class BlockNotConfiguredException : public HumpException< std::string >
{
public:
	BlockNotConfiguredException(const std::string& unknownId) : HumpException< std::string >(unknownId) { }

	const char* what()
	{
		return "Block is not configured";
	}
};



class DuplicatePortNameException : public HumpException< std::string >
{
public:
	DuplicatePortNameException(const std::string& unknownId) : HumpException< std::string >(unknownId) { }

	const char* what()
	{
		return "Port name already exists";
	}
};



class DuplicateBlockNameException : public HumpException< std::string >
{
public:
	DuplicateBlockNameException(const std::string& unknownId) : HumpException< std::string >(unknownId) { }

	const char* what()
	{
		return "Block name already exists";
	}
};



class SampleTimesMismatchException : public HumpException< std::string >
{
public:
	SampleTimesMismatchException(const std::string& unknownId) : HumpException< std::string >(unknownId) { }

	const char* what()
	{
		return "Sample times of ports do not match";
	}
};



class SignalTypesMismatchException : public HumpException< std::string >
{
public:
	SignalTypesMismatchException(const std::string& unknownId) : HumpException< std::string >(unknownId) { }

	const char* what()
	{
		return "Signal types of ports do not match";
	}
};



class FrameSizesMismatchException : public HumpException< std::string >
{
public:
	FrameSizesMismatchException(const std::string& unknownId) : HumpException< std::string >(unknownId) { }

	const char* what()
	{
		return "Frame sizes of ports do not match";
	}
};



class IncompatibleTypesException : public HumpException< std::string >
{
public:
	IncompatibleTypesException(const std::string& unknownId) : HumpException< std::string >(unknownId) { }

	const char* what()
	{
		return "Incompatible types in assignement";
	}
};



class ParameterCountMismatchException : public HumpException< std::string >
{
public:
	ParameterCountMismatchException(const std::string& unknownId) : HumpException< std::string >(unknownId) { }

	const char* what()
	{
		return "Wrong number of parameters";
	}
};



template < class IdentifierType, class ProductType >
class BlockFactoryError
{
protected:
	ProductType* OnUnknownType(const IdentifierType& id)
	{
		throw InvalidBlockIdException(id);
	}
};

#endif
