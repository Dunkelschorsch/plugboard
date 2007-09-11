#ifndef _FACTORY_HPP
#define _FACTORY_HPP

#include <map>
#include <stdexcept>
#include <iostream>

template < class IdentifierType, class ProductType >
class DefaultFactoryError
{
public:
	virtual ~DefaultFactoryError() { }

	class Exception : public std::exception
	{
	public:
		Exception(const IdentifierType& unknownId) : unknownId_(unknownId) { }

		const char* what()
		{
			return "Unknown object type passed to Factory.";
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


template
<
	class AbstractProduct,
	typename IdentifierType,
	typename ProductCreator = AbstractProduct* (*)(),
	template< typename, class >
		class FactoryErrorPolicy = DefaultFactoryError
>
class Factory : public FactoryErrorPolicy< IdentifierType, AbstractProduct >
{
public:
	Factory() : f_() { }

	bool Register(const IdentifierType& id, ProductCreator creator)
	{
		return f_.insert(std::make_pair(id, creator)).second;
	}

	ProductCreator & CreationFunction(const IdentifierType& id)
	{
		typename factory_map_t::iterator it = f_.find(id);
		if(it != f_.end())
		{
			return (it->second);
		}
	}

	AbstractProduct * CreateObject(const IdentifierType& id)
	{
		typename factory_map_t::iterator it = f_.find(id);
		if(it != f_.end())
		{
			return (it->second)();
		}
		return OnUnknownType(id);
	}

private:
	typedef std::map< IdentifierType, ProductCreator > factory_map_t;
	
	factory_map_t f_;
};

#endif // _FACTORY_HPP
