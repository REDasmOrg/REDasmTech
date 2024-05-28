#pragma once

#ifdef __cplusplus
#define REDASM_EXTERN_C extern "C"
#else
#define REDASM_EXTERN_C
#endif

#if defined _MSC_VER // Defined by Visual Studio
#define REDASM_IMPORT REDASM_EXTERN_C __declspec(dllimport)
#define REDASM_EXPORT REDASM_EXTERN_C __declspec(dllexport)
#else
#if __GNUC__ >= 4 // Defined by GNU C Compiler. Also for C++
#define REDASM_IMPORT REDASM_EXTERN_C __attribute__((visibility("default")))
#define REDASM_EXPORT REDASM_EXTERN_C __attribute__((visibility("default")))
#else
#define REDASM_IMPORT REDASM_EXTERN_C
#define REDASM_EXPORT REDASM_EXTERN_C
#endif
#endif
