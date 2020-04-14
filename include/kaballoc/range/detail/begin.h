#pragma once

#include "kaballoc/core/size_t.h"

namespace kab::range
{
	namespace detail
	{
		struct begin_invoker
		{
			template<typename Range>
			auto operator()(Range&& r) const noexcept -> decltype(r.begin())
			{
				return r.begin();
			}

			template<typename Range>
			auto operator()(Range&& r) const noexcept -> decltype(begin(r))
			{
				return begin(r);
			}

			template<typename T, size_t N>
			auto operator()(T(&arr)[N]) const noexcept -> T*
			{
				return arr;
			}

			template<typename T, size_t N>
			auto operator()(T const (&arr)[N]) const noexcept -> T const*
			{
				return arr;
			}
		};
	}

	inline constexpr detail::begin_invoker begin;
}
