#pragma once

#include "memory/memory_common.h"

namespace kab
{
	// Strong typedef for an alignment value
	enum class align_t : size_t {};

	// The biggest natural alignment. Any alignment value above this is considered "over-alignment"
	inline constexpr align_t max_align_v{ alignof(long double) };
}