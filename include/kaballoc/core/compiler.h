#pragma once

#if defined(_MSC_VER)
#  define KAB_COMPILER_MSVC 1
#elif defined(__clang__)
#  define KAB_COMPILER_CLANG 1
#elif defined(__GNUC__)
#  define KAB_COMPILER_GCC 1
#endif

#if !defined(KAB_COMPILER_MSVC)
#  define KAB_COMPILER_MSVC 0
#endif

#if !defined(KAB_COMPILER_CLANG)
#  define KAB_COMPILER_CLANG 0
#endif

#if !defined(KAB_COMPILER_GCC)
#  define KAB_COMPILER_GCC 0
#endif
