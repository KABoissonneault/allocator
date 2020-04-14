#pragma once

#include "kaballoc/core/ptrdiff_t.h"

namespace kab::range
{
	template<typename T>
	ptrdiff_t distance(T* it, T* sent)
	{
		return sent - it;
	}
}
