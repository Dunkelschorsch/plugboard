#ifndef _EXCEPTIONS_HPP
#define _EXCEPTIONS_HPP

#include <stdexcept>


template < class IdentifierType, class ProductType >
class BlockFactoryError
{
public:
	class Exception : public std::exception
	{
	public:
		Exception(const IdentifierType& unknownId) : unknownId_(unknownId) { }

		virtual const char* what()
		{
			return "Unknown block name passed";
		}

		const IdentifierType GetId()
		{
			return unknownId_;
		};

		~Exception() throw() { };
	private:
		IdentifierType unknownId_;
	};

protected:
	ProductType* OnUnknownType(const IdentifierType& id)
	{
		throw Exception(id);
	}
};


typedef BlockFactoryError< std::string, Block >::Exception UnknownBlockException;



struct non_existant_port_error : public std::runtime_error
{
	non_existant_port_error(const std::string& arg) : std::runtime_error(arg) {}

	inline const char *which() { return what(); }
};



struct block_not_configured_error : public std::runtime_error
{
	block_not_configured_error(const std::string& arg) : std::runtime_error(arg) {}

	inline const char *which() { return what(); }
};



struct duplicate_block_name_error : public std::runtime_error
{
	duplicate_block_name_error(const std::string& arg) : std::runtime_error(arg) {}

	inline const char *which() { return what(); }
};



struct duplicate_port_name_error : public std::runtime_error
{
	duplicate_port_name_error(const std::string& arg) : std::runtime_error(arg) {}

	inline const char *which() { return what(); }
};

#endif
