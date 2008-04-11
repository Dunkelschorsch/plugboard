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
