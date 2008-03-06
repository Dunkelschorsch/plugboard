#ifndef EXCEPTION_BASE_HPP
#define EXCEPTION_BASE_HPP


#include <stdexcept>
#include "visibility.hpp"


template < class IdentifierType >
class DSOEXPORT HumpException : public std::exception
{
public:
	HumpException(const IdentifierType& unknownId)
		: unknownId_(unknownId) { }

	const char* what() const throw()
	{
		return do_what();
	}

	const IdentifierType get_id() const throw()
	{
		return unknownId_;
	};

	virtual ~HumpException() throw() { };

private:
	IdentifierType unknownId_;

	virtual const char* do_what() const = 0;
};

#endif // EXCEPTION_BASE_HPP
