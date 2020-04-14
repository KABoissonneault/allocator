#pragma once

namespace kab
{
	template<typename T, typename U>
	auto min(T a, U b) -> decltype(true ? T() : U())
	{
		return a < b ? a : b;
	}

	template<typename T, typename U>
	auto max(T a, U b) -> decltype(true ? T() : U())
	{
		return a < b ? b : a;
	}
}
