#ifndef _FACTORY_HPP
#define _FACTORY_HPP

#include <map>


template
<
	class AbstractProduct,
	typename IdentifierType,
	typename ProductCreator = AbstractProduct* (*)()
>
class Factory
{
public:
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

	}

private:
	typedef std::map< IdentifierType, ProductCreator > factory_map_t;
	
	factory_map_t f_;
};

#endif // _FACTORY_HPP
