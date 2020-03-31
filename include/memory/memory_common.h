#pragma once

namespace kab
{
	using size_t = decltype(sizeof(void*));
	inline constexpr size_t size_t_max_v = -1;
	using byte = unsigned char;
}