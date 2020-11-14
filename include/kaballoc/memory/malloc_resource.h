#pragma once

#include "kaballoc/memory/resource.h"
#include "kaballoc/core/compiler.h"

#if KAB_COMPILER_MSVC
#include <corecrt_malloc.h>
#include <cassert>
#else
#include <cstdlib>
#endif

namespace kab
{
#if KAB_COMPILER_MSVC
	struct malloc_resource
	{
		[[nodiscard]] byte_span allocate(size_t size, align_t align)
		{
			return { static_cast<byte*>(_aligned_malloc(size, static_cast<size_t>(align))), size };
		}

		[[nodiscard]] byte_span over_allocate(size_t size, align_t align)
		{
			size_t const a = static_cast<size_t>(align);
			byte* const ptr = static_cast<byte*>(_aligned_malloc(size, a));
			assert(ptr != nullptr);
			size_t const n = _aligned_msize(ptr, a, 0 /*offset*/);
			return { ptr, n };
		}

		void deallocate(byte_span s, align_t align)
		{
			(void)align;
			_aligned_free(s.data);
		}
	};
#else
#error "malloc_resource.h: implement non-MSVC"
#endif 
}
