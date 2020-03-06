#pragma once

#include <cstddef>

namespace kab
{
	struct byte_span 
	{
		std::byte* data;
		size_t size;
	};

	inline std::byte* data(byte_span span) noexcept 
	{
		return span.data;
	}

	inline size_t size(byte_span span) noexcept 
	{
		return span.size;
	}

	inline std::byte* begin(byte_span span) noexcept 
	{
		return span.data;
	}

	inline std::byte* end(byte_span span) noexcept 
	{
		return span.data + span.size;
	}
}