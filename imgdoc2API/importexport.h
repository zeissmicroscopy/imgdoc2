#pragma once

#ifdef LIBIMGDOC2_EXPORTS
#ifdef __GNUC__
#define LIBIMGDOC2_API __attribute__ ((visibility ("default")))
#define EXTERNAL_API(_returntype_)  extern "C"  _returntype_ __attribute__ ((visibility ("default"))) __attribute__((stdcall))
#else
#define LIBIMGDOC2_API __declspec(dllexport)
#define EXTERNAL_API(_returntype_)  extern "C"  _returntype_ LIBIMGDOC2_API  __stdcall 
#endif
#else
#ifdef __GNUC__
#define LIBIMGDOC2_API
#else
#define LIBIMGDOC2_API __declspec(dllimport)
#endif
#endif

