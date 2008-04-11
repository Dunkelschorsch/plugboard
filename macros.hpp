#ifndef MACROS_HPP
#define MACROS_HPP

#define PB_D(CLASS) CLASS##Impl * const d = d_func();

#define PB_DECLARE_PRIVATE(CLASS)\
friend class CLASS##Impl;\
inline CLASS##Impl *d_func() { return reinterpret_cast< CLASS##Impl* >(d_ptr); }\
inline const CLASS##Impl *d_func() const { return reinterpret_cast< CLASS##Impl* >(d_ptr); }

#define PB_DECLARE_PROTECTED(CLASS)\
CLASS##Impl * const d_ptr;\
CLASS(CLASS##Impl &dd);

#endif // MACROS_HPP
