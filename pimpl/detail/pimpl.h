#ifndef PIMPL_DETAIL_HEADER_VB
#define PIMPL_DETAIL_HEADER_VB

// Copyright 2001-2007 Vladimir Batov.
// Use, modification and distribution are subject to the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt.

// Series of constructors transferring parameters down to the implementation class.
// That is done to encapsulate object construction inside this class and, consequently,
// *fully* automate memory management (not just deletion).

// 'const' variations are to preserve const-ness during transfer.
// For example, something like
//      Foo foo;
//      detail(foo, ...);
// can be handled by template<A1> detail(A1&).
// However, passing parameteres via a temporary like
//      detail(Foo(), ...)
// requires template<A1> detail(A1 const&).
//
// See http://www.ddj.com/dept/cpp/184401243.
//

#undef  __PIMPL_CONSTRUCTOR__
#define __PIMPL_CONSTRUCTOR__(c1, c2)   \
    template<class A1, class A2>        \
    detail(A1 c1& a1, A2 c2& a2)        \
    : impl_(new implementation(a1,a2)) {}

__PIMPL_CONSTRUCTOR__ (     ,     )
__PIMPL_CONSTRUCTOR__ (     ,const)
__PIMPL_CONSTRUCTOR__ (const,     )
__PIMPL_CONSTRUCTOR__ (const,const)

#undef  __PIMPL_CONSTRUCTOR__
#define __PIMPL_CONSTRUCTOR__(c1, c2, c3)   \
    template<class A1, class A2, class A3>  \
    detail(A1 c1& a1, A2 c2& a2, A3 c3& a3) \
    : impl_(new implementation(a1,a2,a3)) {}

__PIMPL_CONSTRUCTOR__ (     ,     ,     )
__PIMPL_CONSTRUCTOR__ (     ,     ,const)
__PIMPL_CONSTRUCTOR__ (     ,const,     )
__PIMPL_CONSTRUCTOR__ (     ,const,const)
__PIMPL_CONSTRUCTOR__ (const,     ,     )
__PIMPL_CONSTRUCTOR__ (const,     ,const)
__PIMPL_CONSTRUCTOR__ (const,const,     )
__PIMPL_CONSTRUCTOR__ (const,const,const)

#undef  __PIMPL_CONSTRUCTOR__
#define __PIMPL_CONSTRUCTOR__(c1, c2, c3, c4)           \
    template<class A1, class A2, class A3, class A4>    \
    detail(A1 c1& a1, A2 c2& a2, A3 c3& a3, A4 c4& a4)  \
    : impl_(new implementation(a1,a2,a3,a4)) {}

__PIMPL_CONSTRUCTOR__ (     ,     ,     ,     )
__PIMPL_CONSTRUCTOR__ (     ,     ,     ,const)
__PIMPL_CONSTRUCTOR__ (     ,     ,const,     )
__PIMPL_CONSTRUCTOR__ (     ,     ,const,const)
__PIMPL_CONSTRUCTOR__ (     ,const,     ,     )
__PIMPL_CONSTRUCTOR__ (     ,const,     ,const)
__PIMPL_CONSTRUCTOR__ (     ,const,const,     )
__PIMPL_CONSTRUCTOR__ (     ,const,const,const)
__PIMPL_CONSTRUCTOR__ (const,     ,     ,     )
__PIMPL_CONSTRUCTOR__ (const,     ,     ,const)
__PIMPL_CONSTRUCTOR__ (const,     ,const,     )
__PIMPL_CONSTRUCTOR__ (const,     ,const,const)
__PIMPL_CONSTRUCTOR__ (const,const,     ,     )
__PIMPL_CONSTRUCTOR__ (const,const,     ,const)
__PIMPL_CONSTRUCTOR__ (const,const,const,     )
__PIMPL_CONSTRUCTOR__ (const,const,const,const)

#undef  __PIMPL_CONSTRUCTOR__
#define __PIMPL_CONSTRUCTOR__(c1, c2, c3, c4, c5)                   \
    template<class A1, class A2, class A3, class A4, class A5>      \
    detail(A1 c1& a1, A2 c2& a2, A3 c3& a3, A4 c4& a4, A5 c5& a5)   \
    : impl_(new implementation(a1,a2,a3,a4,a5)) {}

__PIMPL_CONSTRUCTOR__ (     ,     ,     ,     ,     )
__PIMPL_CONSTRUCTOR__ (     ,     ,     ,     ,const)
__PIMPL_CONSTRUCTOR__ (     ,     ,     ,const,     )
__PIMPL_CONSTRUCTOR__ (     ,     ,     ,const,const)
__PIMPL_CONSTRUCTOR__ (     ,     ,const,     ,     )
__PIMPL_CONSTRUCTOR__ (     ,     ,const,     ,const)
__PIMPL_CONSTRUCTOR__ (     ,     ,const,const,     )
__PIMPL_CONSTRUCTOR__ (     ,     ,const,const,const)
__PIMPL_CONSTRUCTOR__ (     ,const,     ,     ,     )
__PIMPL_CONSTRUCTOR__ (     ,const,     ,     ,const)
__PIMPL_CONSTRUCTOR__ (     ,const,     ,const,     )
__PIMPL_CONSTRUCTOR__ (     ,const,     ,const,const)
__PIMPL_CONSTRUCTOR__ (     ,const,const,     ,     )
__PIMPL_CONSTRUCTOR__ (     ,const,const,     ,const)
__PIMPL_CONSTRUCTOR__ (     ,const,const,const,     )
__PIMPL_CONSTRUCTOR__ (     ,const,const,const,const)
__PIMPL_CONSTRUCTOR__ (const,     ,     ,     ,     )
__PIMPL_CONSTRUCTOR__ (const,     ,     ,     ,const)
__PIMPL_CONSTRUCTOR__ (const,     ,     ,const,     )
__PIMPL_CONSTRUCTOR__ (const,     ,     ,const,const)
__PIMPL_CONSTRUCTOR__ (const,     ,const,     ,     )
__PIMPL_CONSTRUCTOR__ (const,     ,const,     ,const)
__PIMPL_CONSTRUCTOR__ (const,     ,const,const,     )
__PIMPL_CONSTRUCTOR__ (const,     ,const,const,const)
__PIMPL_CONSTRUCTOR__ (const,const,     ,     ,     )
__PIMPL_CONSTRUCTOR__ (const,const,     ,     ,const)
__PIMPL_CONSTRUCTOR__ (const,const,     ,const,     )
__PIMPL_CONSTRUCTOR__ (const,const,     ,const,const)
__PIMPL_CONSTRUCTOR__ (const,const,const,     ,     )
__PIMPL_CONSTRUCTOR__ (const,const,const,     ,const)
__PIMPL_CONSTRUCTOR__ (const,const,const,const,     )
__PIMPL_CONSTRUCTOR__ (const,const,const,const,const)

#undef __PIMPL_CONSTRUCTOR__

// Implement more when needed.

#endif // PIMPL_DETAIL_HEADER_VB
