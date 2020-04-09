#pragma once

#include "kaballoc/memory/memory_common.h"

namespace kab
{
	struct byte_span 
	{
		byte* data;
		size_t size;
	};

	[[nodiscard]] inline byte* data(byte_span span) noexcept
	{
		return span.data;
	}

	[[nodiscard]] inline size_t size(byte_span span) noexcept
	{
		return span.size;
	}

	[[nodiscard]] inline byte* begin(byte_span span) noexcept
	{
		return span.data;
	}

	[[nodiscard]] inline byte* end(byte_span span) noexcept
	{
		return span.data + span.size;
	}
}