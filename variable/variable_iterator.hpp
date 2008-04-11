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

#ifndef VARIABLE_ITERATOR_HPP
#define VARIABLE_ITERATOR_HPP

#include <cassert>
#include <boost/iterator/iterator_facade.hpp>

namespace plugboard
{
	template< typename T, class VarType=Variable >
	class variable_iterator :
		public boost::iterator_facade<
			variable_iterator< T, VarType >,
			T,
			boost::forward_traversal_tag
		>
	{
	public:
		variable_iterator() : var_(0), value_(0), pos_(0) { }

		explicit variable_iterator(VarType& v) : var_(v),  value_(0), pos_(0)
		{
			increment();
		}

		explicit variable_iterator(VarType& v, size_t pos) : var_(v), value_(0), pos_(1+pos) { }

		variable_iterator< T, VarType > make_end()
		{
			variable_iterator< T, VarType > tmp = variable_iterator< T, VarType >(var_);
			tmp.pos_ = var_.size()+1;
			return tmp;
		}

	private:
		friend class boost::iterator_core_access;

		void increment()
		{
			assert(pos_ <= var_.size());

			if(var_.get_type() == string || var_.get_type() == complex)
			{
				if(pos_ == var_.size())
				{
					pos_++;
					return;
				}
				value_ = static_cast< T** >(var_.data())[pos_++];
	#ifndef NDEBUG
				std::cout << "size: " << std::endl << var_.size();
				std::cout << "fetching value..." << *value_ << std::endl;
	#endif
			}
			else
				value_ = &static_cast< T* >(var_.data())[pos_++];
		}

		bool equal(const variable_iterator& other) const
		{
			return this->pos_ == other.pos_;
		}

		T& dereference() const
		{
			return *value_;
		}

		VarType& var_;
		T * value_;
		size_t pos_;
	};
} // namespace plugboard

#endif // VARIABLE_ITERATOR_HPP
