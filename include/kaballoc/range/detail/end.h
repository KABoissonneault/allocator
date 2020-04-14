#pragma once

#include "kaballoc/core/size_t.h"

namespace kab::range
{
	namespace detail
	{
		struct end_invoker
		{
			template<typename Range>
			auto operator()(Range&& r) const noexcept -> decltype(r.end())
			{
				return r.end();
			}

			template<typename Range>
			auto operator()(Range&& r) const noexcept -> decltype(end(r))
			{
				return end(r);
			}

			template<typename T, size_t N>
			auto operator()(T(&arr)[N]) const noexcept -> T*
			{
				return arr + N;
			}

			template<typename T, size_t N>
			auto operator()(T const (&arr)[N]) const noexcept -> T const*
			{
				return arr + N;
			}
		};
	}

	inline constexpr detail::end_invoker end;
}
