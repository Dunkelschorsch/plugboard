/*!
 * \file
 * \brief
 * \author Armin Schmidt
 *
 * ----------------------------------------------------------------------------
 *
 * PlugBoard - A versatile communication simulation framework
 * Copyright (C) 2007-2008  Armin Schmidt
 *
 * This file is part of PlugBoard.
 *
 * PlugBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PlugBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PlugBoard.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ----------------------------------------------------------------------------
 */

#ifndef FACTORY_HPP
#define FACTORY_HPP

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

		const char* what() const
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
	ProductType* OnUnknownType(const IdentifierType& id) const
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


	AbstractProduct * CreateObject(const IdentifierType& id)
	{
		typename factory_map_t::const_iterator it = f_.find(id);
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

#endif // FACTORY_HPP
