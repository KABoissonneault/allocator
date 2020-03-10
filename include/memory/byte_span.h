#pragma once

#include <cstddef>

namespace kab
{
	struct byte_span 
	{
		std::byte* data;
		size_t size;
	};

	[[nodiscard]] inline std::byte* data(byte_span span) noexcept
	{
		return span.data;
	}

	[[nodiscard]] inline size_t size(byte_span span) noexcept
	{
		return span.size;
	}

	[[nodiscard]] inline std::byte* begin(byte_span span) noexcept
	{
		return span.data;
	}

	[[nodiscard]] inline std::byte* end(byte_span span) noexcept
	{
		return span.data + span.size;
	}
}