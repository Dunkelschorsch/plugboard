#ifndef BLOCK_DYNAMIC_HPP
#define BLOCK_DYNAMIC_HPP

#include <boost/mpl/map.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/int.hpp>

#include "types/base.hpp"
#include "port/port_in.hpp"


#define PB_DYNAMIC_BLOCK\
	friend class Dynamic< PlugBoardBlock >;\
	template< class T >\
	void dynamic_init();\
	template< class T >\
	void dynamic_delete();\
	template< class T >\
	void dynamic_process();\
	\
	void process()\
	{\
		(this->*proc)();\
	}



class PlugBoardBlock;

namespace plugboard
{
	namespace mpl = boost::mpl;
	typedef mpl::map<
		mpl::pair< uint8_t,   mpl::int_< uint8 > >,
		mpl::pair< int8_t,    mpl::int_< int8 > >,
		mpl::pair< uint16_t,  mpl::int_< uint16 > >,
		mpl::pair< int16_t,   mpl::int_< int16 > >,
		mpl::pair< uint32_t,  mpl::int_< uint32 > >,
		mpl::pair< int32_t,   mpl::int_< int32 > >,
		mpl::pair< real_t,    mpl::int_< real > >,
		mpl::pair< complex_t, mpl::int_< complex > >,
		mpl::pair< empty_t,   mpl::int_< empty > >
	> types_m;


	template< class ParentT >
	class Dynamic
	{
	public:
		typedef Dynamic< ParentT > DynamicBlock;
		typedef void(DynamicBlock::*proc_func_t)();

		Dynamic(ParentT* const);
		~Dynamic();

		template< class allowedT >
		void initialize(const InPort* const p);

		inline void initialize(const InPort* const p);

		proc_func_t proc;

	private:
		template< class T >
		inline void dynamic_init();

		template< class T >
		inline void dynamic_process();

		template< class T >
		inline void dynamic_delete();

		proc_func_t destroy;

		ParentT* const parent_;

		struct dyn_init_dispatch
		{
			dyn_init_dispatch(const type_t dyn_type, Dynamic< ParentT >* const self)
				: type_(dyn_type), self_(self) { }

			template< class T >
			inline void operator()(T)
			{
				if(type_ == typename T::second::type())
				{
					self_->dynamic_init< typename T::first >();
				}
			}

			const type_t type_;
			Dynamic< ParentT >* const self_;
		};
	};


	template< class ParentT >
	Dynamic< ParentT >::Dynamic(ParentT* const parent)
		: parent_(parent)
	{

	}


	template< class ParentT >
	Dynamic< ParentT >::~Dynamic()
	{
		if(parent_->is_initialized())
			(this->*destroy)();
	}


	template< class ParentT >
	void Dynamic< ParentT >::initialize(const InPort* const p)
	{
		initialize< boost::mpl::set< int32_t, real_t, complex_t > >(p);
	}


	template< class ParentT >
	template< class allowedT >
	void Dynamic< ParentT >::initialize(const InPort* const p)
	{
		using namespace boost::mpl;

		typedef typename copy<
			iterator_range< begin< types_m >::type, end< types_m >::type >,
			inserter<
				map< >,
				if_<
					has_key< typename allowedT::type, first< deref< _2 > > >,
					insert< _1, deref< _2 > >,
					_1
				>
			>
		>::type mm;

		boost::mpl::for_each<
			iterator_range<
				typename begin< mm >::type, typename end< mm >::type
			>
		>(dyn_init_dispatch(p->get_type(), this));
	}


	template< class ParentT >
	template< class T >
	void Dynamic< ParentT >::dynamic_init()
	{
		parent_->dynamic_init< T >();

		proc = &Dynamic< PlugBoardBlock >::dynamic_process< T >;
		destroy = &Dynamic< PlugBoardBlock >::dynamic_delete< T >;
	}


	template< class ParentT >
	template< class T >
	void Dynamic< ParentT >::dynamic_delete()
	{
		parent_->dynamic_delete< T >();
	}


	template< class ParentT >
	template< class T >
	void Dynamic< ParentT >::dynamic_process()
	{
		parent_->dynamic_process< T >();
	}
}

#endif // BLOCK_DYNAMIC_HPP

