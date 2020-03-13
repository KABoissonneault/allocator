#pragma once

#include "memory/byte_span.h"
#include "memory/resource.h"

#include <new>

namespace kab
{
	struct new_resource
	{
		[[nodiscard]] byte_span allocate(size_t size, align_t align)
		{
			return { static_cast<byte*>(::operator new(size, static_cast<std::align_val_t>(align))), size };
		}

		void deallocate(byte_span s, align_t align)
		{
			::operator delete(s.data, s.size, static_cast<std::align_val_t>(align));
		}
	};
}