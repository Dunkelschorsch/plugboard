#ifndef VISIBILITY_HPP
#define VISIBILITY_HPP


// #ifdef _MSC_VER
//   #ifdef BUILDING_DLL
//     #define DSOEXPORT __declspec(dllexport)
//   #else
//     #define DSOEXPORT __declspec(dllimport)
//   #endif
//   #define DLLLOCAL
// #else
//   #ifdef HAVE_GCCVISIBILITYPATCH
//     #define DSOEXPORT __attribute__ ((visibility("default")))
//     #define DLLLOCAL __attribute__ ((visibility("hidden")))
//   #else
//     #define DSOEXPORT
//     #define DLLLOCAL
//   #endif
// #endif


#ifdef DISABLE_VISIBILITY_ALTOGETHER
    #define DSOEXPORT
    #define DSOLOCAL
#else
    #define DSOEXPORT __attribute__ ((visibility("default")))
    #define DSOLOCAL __attribute__ ((visibility("hidden")))
#endif

#endif // VISIBILITY_HPP
