#ifndef VB_PIMPL_GENERALIZATION_HEADER
#define VB_PIMPL_GENERALIZATION_HEADER

// Copyright (c) 2001 Peter Dimov and Multi Media Ltd.
// Copyright (c) 2007 Vladimir Batov.
// Use, modification and distribution are subject to the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt.

//#include <auxiliary/forward.h>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/type_traits.hpp>

template<class U>
struct pimpl
{
    struct implementation;
    template<class> class impl_ptr;
    template<template<class> class> class detail;
    typedef detail<impl_ptr> value_semantics;
    typedef detail<boost::shared_ptr> pointer_semantics;
};

template<class U>
template<class T>
class pimpl<U>::impl_ptr
{
    public:

   ~impl_ptr () { traits_->destroy(p_); }
    impl_ptr () : traits_(deep_copy()), p_(0) {}
    impl_ptr (T* p) : traits_(deep_copy()), p_(p) {}
    impl_ptr (impl_ptr const& that) : traits_(that.traits_), p_(traits_->copy(that.p_)) {}
    impl_ptr& operator=(impl_ptr const& that) { traits_->assign(p_, that.p_); return *this; }

    void swap(impl_ptr& that) { std::swap(p_, that.p_), std::swap(traits_, that.traits_); }

    T* get() const { return p_; }

    private:

    struct traits
    {
        virtual ~traits() {}
        virtual void destroy (T*&) const =0;
        virtual T*      copy (T const*) const =0;
        virtual void  assign (T*&, T const*) const =0;
    };
    struct deep_copy : public traits
    {
        virtual void destroy (T*& p) const { boost::checked_delete(p); p = 0; }
        virtual T*      copy (T const* p) const { return p ? new T(*p) : 0; }
        virtual void  assign (T*& a, T const* b) const
        {
            if      ( a ==  b);
            else if ( a &&  b) *a = *b;
            else if (!a &&  b) a = copy(b);
            else if ( a && !b) destroy(a);
        }
        operator traits const*() { static deep_copy impl; return &impl; }
    };

    traits const* traits_;
    T*                 p_;
};

struct pimpl_base_ {};

// TODO. Think if we could use a deleter
//template<class T, class D/*eleter*/>
template<class T>
template<template<class> class Policy>
class pimpl<T>::detail : pimpl_base_
{

    public:

    typedef detail<Policy> base;
    void internal_bool() {};
    typedef void (base::*bool_type)();

    // For simple non-polymorphic cases it's natural to call Foo::null().
    // For polymorphic classes (like Base and Derived) Derived::null() will still return an instance
    // of Base. Therefore, to get a null instance of the Derived type null<Derived>() needs to be
    // called instead.
    static T null() { return null<T>(); }

    // Function is protected by enable_if to be called only for classes derived from pimpl.
    // That is, a call to Pimpl::null<NotDerived>() will not compile.
    template<class Derived>
    static
    typename boost::enable_if<boost::is_base_of<base, Derived>, Derived>::type
    null()
    {
        null_type null_value;
        base instance(null_value);
        return *(Derived*) &instance;
    }

    // detail::op==() simply transfers the comparison down to the implementation policy --
    // boost::shared_ptr or impl_ptr. Consequently, a shared_ptr-based pimpl is comparable as
    // shared_ptr has op==(). However, an impl_ptr-based pimpl (with value semantics) is not
    // comparable by default (the standard behavior) as impl_ptr::op==() is not implemented.
    // If a value-semantics class T needs to be comparable, then it has to explicitly provide
    // T::operator==(T const&) as part of its public interface. Trying to call
    // this detail::op==() for impl_ptr-based pimpl will fail to compile (impl_ptr::op==()
    // is not implemented) and will indicate that the user forgot to declare
    // T::operator==(T const&).
    bool operator==(T const& that) const { return impl_ == that.impl_; }

    // Implementing an implicit conversion to bool (operator bool() const) is very convenient and
    // tempting to be deployed in constructs like "if (pimpl)" and "if (!pimpl)" (without explicit
    // operator!()). However, DO NOT IMPLEMENT "operator bool()" as that conversion kicks in way
    // too often and unexpectedly. Like "pimpl + 1", "1 + pimpl" or potentially during
    // boost::lexical_cast<std::string>(pimpl) (if there are no >> and << defined for streams).
    // Consequently, that implicit-conversion-to-bool functionality is to be implemented in an
    // indirect way via an implicit conversion to some pointer type. The best type for the purpose
    // appears to be a pointer to a member function. For more see the chapter 7.7 in Alexandrescu's
    // "Modern C++ Design" and how that functionality is implemented for boost::shared_ptr.
    operator bool_type() const { return impl_.get() ? &base::internal_bool : 0; }

    void swap(T& that) { impl_.swap(that.impl_); }

    protected:

    struct internal_type {};
    struct     null_type {};

    typedef typename pimpl<T>::implementation implementation;

   ~detail () {} // Defined explicitly as 'protected'.
    detail (null_type) {} // Creates an invalid instance (like NULL for raw pointers).
    // The default auto-generated copy constructor and the default assignment operator
    // are just fine (do member-wise copy and assignment respectively).

    // The Pimpl class does NOT behave like a pointer, where, say, const shared_ptr allows to
    // modify the underlying data. The Pimpl class behaves more like a substitute or proxy.
    // Therefore, const instances return const pointers and references.
    //
    // Functions are not public. They only allow access to underlying data for the derived classes
    // in the following manner:
    //      implementation& self = **this;
    // or   implementation const& self = **this; (inside 'const' methods)
    //      self.data = ...;
    implementation const* operator->() const { BOOST_ASSERT(impl_.get()); return  impl_.get(); }
    implementation const& operator *() const { BOOST_ASSERT(impl_.get()); return *impl_.get(); }
    implementation*       operator->()       { BOOST_ASSERT(impl_.get()); return  impl_.get(); }
    implementation&       operator *()       { BOOST_ASSERT(impl_.get()); return *impl_.get(); }

    // Support for
    // a) lazy instantiation and
    // b) run-time polymorphic hierarchies as described in GoF for the Bridge pattern.
    // That describes building two separate class hierarchies -- interfaces and implementations.
    // The interface hierarchy is built as
    //      struct Base : public pimpl<Base>::pointer_semantics
    //      struct Derived : public Base
    //      struct More : public Derived
    //      etc.
    // The implementation hierarchy is built as
    //      typedef pimpl<Base>::implementation BaseImpl;
    //      template<> struct pimpl<Base>::implementation {...}
    //      struct DerivedImpl1 : public BaseImpl { ...}
    //      struct MoreImpl : public DerivedImpl { ...}
    //      etc.
    void reset(implementation* impl) { impl_.reset(impl); }

    // A series of forwarding constructors. These constructors dutifully transfer arguments to
    // the corresponding constructors of the internal 'implementation' class. That is done to
    // encapsulate the construction of the 'implementation' instance inside this class and,
    // consequently, to *fully* automate memory management (rather than just deletion).
    // disable_if<> makes sure that these 1-arg constructors are not called when the copy
    // constructor is needed. Using the 'internal_type' type to make sure that these constructors
    // do not clash with ANY of 2-args constructors.
    detail () : impl_(new implementation()) {}
#undef  __IF_NOT_PIMPL__
#define __IF_NOT_PIMPL__(A) typename boost::disable_if<boost::is_base_of<base, A>, internal_type*>::type =0
    template<class A1> detail(A1 const& a1, __IF_NOT_PIMPL__(A1)) : impl_(new implementation(a1)) {}
    template<class A1> detail(A1&       a1, __IF_NOT_PIMPL__(A1)) : impl_(new implementation(a1)) {}
#undef  __IF_NOT_PIMPL__

#   include "detail/pimpl.h" // More constructors with more parameters.

    private:

    template<class> friend struct pimpl;
    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive& a, unsigned int) { a & BOOST_SERIALIZATION_NVP(impl_); }

    Policy<implementation> impl_;
};

template<class T>
inline
typename boost::enable_if<boost::is_base_of<typename T::base, T>, T>::type
null()
{
    return T::template null<T>();
}

// 1. Catch ALL pimpl-related comparison operators and handle them explicitly.
//    Otherwise, conversion to bool kicks in instead (say, for foo == 5 or 5 == foo)
//    which is most unlikely to be what was needed.
// 2. All op==() and op!=() are implemented via T::op==(T const&) so that the user gets
//    op!=() for free when T::op==() is defined.
// 3. The comparison behavior that appers to be standard is that for, say, "FOO == (int) 5"
//    to succeed, there has to be either FOO::operator==(int) or FOO::operator int().
//    The same goes for "(int) 5 == FOO": there has to be available ::operator==(FOO) or an
//    acceptable conversion. Non-explicit FOO::FOO(int) is not considered to be a conversion.
//    For example, for "struct Foo { Foo(int) }" VS2007 gives
//    binary '==' : 'Foo' does not define this operator or a conversion to a type acceptable to
//                  the predefined operator
//    binary '==' : no global operator found which takes type 'Foo' (or there is no acceptable
//                  conversion)
//    for "foo == 5" and "5 == foo" respectively. GCC gives similar errors.
//    Therefore, we want Pimpl comparisons to behave the same way. Namely, if Pimpl-based Foo
//    defines Foo::operator==(int const&) explicitly, then it'll be deployed. Otherwise,
//    __IF_PIMPL_LEFT__(A,B) operator==() kicks in where we try deploying Foo::operator int()
//    and fail if such a conversion does not exist.

#define __IF_PIMPL_BOTH__(A,B) template<class A, class B> inline typename boost::enable_if_c< boost::is_base_of<pimpl_base_, A>::value &&  boost::is_base_of<pimpl_base_, B>::value, bool>::type
#define __IF_PIMPL_LEFT__(A,B) template<class A, class B> inline typename boost::enable_if_c< boost::is_base_of<pimpl_base_, A>::value && !boost::is_base_of<pimpl_base_, B>::value, bool>::type
#define __IF_PIMPL_RGHT__(A,B) template<class A, class B> inline typename boost::enable_if_c<!boost::is_base_of<pimpl_base_, A>::value &&  boost::is_base_of<pimpl_base_, B>::value, bool>::type

__IF_PIMPL_BOTH__(A,B) operator==(A const& a, B const& b) { return  a.operator==(b); }
__IF_PIMPL_BOTH__(A,B) operator!=(A const& a, B const& b) { return !a.operator==(b); }
__IF_PIMPL_LEFT__(A,B) operator==(A const& a, B const& b) { return  (B(a) == b); }
__IF_PIMPL_LEFT__(A,B) operator!=(A const& a, B const& b) { return !(B(a) == b); }
__IF_PIMPL_RGHT__(A,B) operator==(A const& a, B const& b) { return  (a == A(b)); }
__IF_PIMPL_RGHT__(A,B) operator!=(A const& a, B const& b) { return !(a == A(b)); }

#undef __IF_PIMPL_BOTH__
#undef __IF_PIMPL_LEFT__
#undef __IF_PIMPL_RGHT__

// To deploy boost::serialization for a Pimpl-based class the following steps are needed:
// 1. Add the following serialization DECLARATION to the interface class __CLASS__:
//          private:
//          friend class boost::serialization::access;
//          template<class Archive> void serialize(Archive&, unsigned int);
// 2. Add the following serialization DEFINITION to the implementation class
//    pimpl<__CLASS__>::implementation:
//          void serialize(Archive& a, unsigned int file_version) { ... }
// 3. Add the following macro AFTER pimpl<__CLASS__>::implementation declaration.
// 4. For more info see http://www.boost.org/libs/serialization/doc/index.html
//    and then go to "Case Studies/PIMPL"
#define BOOST_SERIALIZATION_PIMPL(__CLASS__)                \
                                                            \
    /* Serialization definition for __CLASS__ */            \
    template<class Archive>                                 \
    void                                                    \
    __CLASS__::serialize(Archive& a, unsigned int)          \
    {                                                       \
        a & boost::serialization::base_object<base>(*this); \
    }                                                       \
    /* Explicit instantiation of the serialization code */  \
    /* Add other archives when you use them */              \
    template void __CLASS__::serialize(                     \
        boost::archive::binary_iarchive&, unsigned int);    \
    template void __CLASS__::serialize(                     \
        boost::archive::binary_oarchive&, unsigned int);

#endif // VB_PIMPL_GENERALIZATION_HEADER
