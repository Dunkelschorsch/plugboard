#ifndef _MACROS_HPP
#define _MACROS_HPP

#define H_D(CLASS) CLASS##Impl * const d = d_func();

#define H_DECLARE_PRIVATE(CLASS)\
friend class CLASS##Impl;\
inline CLASS##Impl *d_func() { return d_ptr; }\
inline const CLASS##Impl *d_func() const { return d_ptr; }

#define H_DECLARE_PROTECTED(CLASS)\
CLASS##Impl * const d_ptr;\
CLASS(CLASS##Impl &dd);

#endif // _MACROS_HPP
