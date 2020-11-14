#pragma once

#include <iterator>

namespace kab
{
	template<typename Range>
	class move_view
	{
		Range* m_range;

	public:
		move_view(Range& range)
			: m_range(&range)
		{

		}

		[[nodiscard]] std::move_iterator<typename Range::iterator> begin() const noexcept { return std::make_move_iterator(m_range->begin()); }
		[[nodiscard]] std::move_iterator<typename Range::iterator> end() const noexcept { return std::make_move_iterator(m_range->end()); }
	};
}
