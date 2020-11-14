#pragma once

#include "kaballoc/core/compiler.h"

#if KAB_COMPILER_MSVC
#  include <intrin.h>
#  pragma intrinsic (_InterlockedIncrement64)
#  pragma intrinsic (_InterlockedDecrement64)
#  pragma intrinsic (_ReadWriteBarrier)

#  define KAB_ATOMIC_LOAD_RELAXED(v) (+v) 
#  define KAB_ATOMIC_FETCH_INC_UINT64_RELAXED(v) (_InterlockedIncrement64((__int64*)&v) - 1) // "no fence" version only available on ARM
#  define KAB_ATOMIC_FETCH_DEC_UINT64_RELEASE(v) (_InterlockedDecrement64((__int64*)&v) + 1) // "release" version only available on ARM
#  define KAB_ATOMIC_FENCE_ACQUIRE() _ReadWriteBarrier()
#elif KAB_COMPILER_GCC | KAB_COMPILER_CLANG
#  define KAB_ATOMIC_LOAD_RELAXED(v) __atomic_load_n(&v, __ATOMIC_RELAXED)
#  define KAB_ATOMIC_FETCH_INC_UINT64_RELAXED(v) __atomic_fetch_add(&v, 1, __ATOMIC_RELAXED)
#  define KAB_ATOMIC_FETCH_DEC_UINT64_RELEASE(v) __atomic_fetch_sub(&v, 1, __ATOMIC_RELEASE)
#  define KAB_ATOMIC_FENCE_ACQUIRE() __atomic_thread_fence(__ATOMIC_ACQUIRE)
#else
#  error "Atomics not supported on this compiler"
#endif
