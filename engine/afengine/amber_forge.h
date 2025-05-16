#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

/// @def AFAPI
/// Export/import macro for symbols in the AmberForge engine.
/// Use `-DAF_EXPORT` when building the shared library.
#if defined(AF_EXPORT)
    #if defined(_MSC_VER)
        #define AFAPI __declspec(dllexport)
    #else
        #define AFAPI __attribute__((visibility("default")))
    #endif

#else
    #if defined(__MSC_VER)
        #define AFAPI __declspec(dllimport)
    #else
        #define AFAPI
    #endif
#endif

/// @def AF_ASSERT
/// Runtime assertion macro, defaults to `assert` if not overridden.
#if !defined(AF_ASSERT)
    #define AF_ASSERT assert
#endif

/// @def AF_STATIC_ASSERT
/// Compile-time assertion macro, defaults to `static_assert`.
#if !defined(AF_STATIC_ASSERT)
    #define AF_STATIC_ASSERT static_assert
#endif

/// @def AF_INLINE
/// Force-inlining macro based on compiler.
#if !defined(AF_INLINE)
    #if defined(__clang__) || defined(__GNUC__)
        #define AF_INLINE __attribute__((always_inline)) inline
    #elif defined(_MSC_VER)
        #define AF_INLINE __forceinline
    #else
        #define AF_INLINE inline
    #endif
#endif

/// @def AF_NINLINE
/// Prevent-inlining macro based on compiler.
#if !defined(AF_NINLINE)
    #if defined(__clang__) || defined(__GNUC__)
        #define AF_NINLINE __attribute__((noinline))
    #elif defined(_MSC_VER)
        #define AF_NINLINE __declspec(noinline)
    #else
        #define AF_NINLINE
    #endif
#endif

/// @def bool
/// Defines a `bool` type and the `true`/`false` macros for C compilers that do not support `<stdbool.h>`.
/// MSVC before Visual Studio 2013 (_MSC_VER < 1800) and some old C compilers do not support C99.
#if !defined(__cplusplus)
    #if (defined(_MSC_VER) && _MSC_VER < 1800) || (!defined(_MSC_VER) && (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L))
        #ifndef true
            #define true (0 == 0)
        #endif
        #ifndef false
            #define false (0 != 0)
        #endif
typedef unsigned char bool;
    #else
        #include <stdbool.h>
    #endif
#endif

/// @def AF_DEFAULT_ALIGNMENT
#if !defined(AF_DEFAULT_ALIGNMENT)
    #define AF_DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

#define GlobalVariable static
#define Internal static

typedef struct AFgame_config AFgame_config;
struct AFgame_config {
    const char *title;
    uint32_t width;
    uint32_t height;
};