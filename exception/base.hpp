#ifndef EXCEPTION_BASE_HPP
#define EXCEPTION_BASE_HPP


#include <stdexcept>
#include "visibility.hpp"

namespace plugboard
{
	template < class IdentifierType >
	class DSOEXPORT Exception : public std::exception
	{
	public:
		Exception(const IdentifierType& unknownId)
			: unknownId_(unknownId) { }

		const char* what() const throw()
		{
			return do_what();
		}

		const IdentifierType get_id() const throw()
		{
			return unknownId_;
		};

		virtual ~Exception() throw() { };

	private:
		IdentifierType unknownId_;

		virtual const char* do_what() const = 0;
	};
}

#endif // EXCEPTION_BASE_HPP
