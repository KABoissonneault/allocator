#pragma once

#include "memory/memory_common.h"

namespace kab
{
	// Strong typedef for an alignment value
	enum class align_t : size_t {};

	inline constexpr bool operator==(align_t lhs, align_t rhs) noexcept
	{
		return static_cast<size_t>(lhs) == static_cast<size_t>(rhs);
	}

	inline constexpr bool operator!=(align_t lhs, align_t rhs) noexcept
	{
		return static_cast<size_t>(lhs) != static_cast<size_t>(rhs);
	}

	inline constexpr bool operator<(align_t lhs, align_t rhs) noexcept
	{
		return static_cast<size_t>(lhs) < static_cast<size_t>(rhs);
	}

	inline constexpr bool operator<=(align_t lhs, align_t rhs) noexcept
	{
		return static_cast<size_t>(lhs) <= static_cast<size_t>(rhs);
	}

	inline constexpr bool operator>(align_t lhs, align_t rhs) noexcept
	{
		return static_cast<size_t>(lhs) > static_cast<size_t>(rhs);
	}

	inline constexpr bool operator>=(align_t lhs, align_t rhs) noexcept
	{
		return static_cast<size_t>(lhs) >= static_cast<size_t>(rhs);
	}

	// The biggest natural alignment. Any alignment value above this is considered "over-alignment"
	inline constexpr align_t max_align_v{ alignof(long double) };

	inline constexpr bool is_power_of_two(size_t n) noexcept
	{
		return n != 0 && ((n & (n - 1)) == 0);
	}
}