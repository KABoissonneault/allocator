#pragma once

#include "kaballoc/trait/relocatable.h"
#include "kaballoc/range/detail/distance.h"

#include <string.h>

namespace kab
{
	template<typename T>
	void uninitialized_relocate(T* it, T* sent, T* dst)
	{
		static_assert(is_trivially_relocatable_v<T>, "A trivially relocatable type is required");
		memcpy(dst, it, range::distance(it, sent) * sizeof(T));
	}
}
