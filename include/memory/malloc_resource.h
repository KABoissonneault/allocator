#pragma once

#include "memory/byte_span.h"
#include <cstdlib>

namespace kab
{
	// MonostateMemoryResource implementation using malloc to fetch from the global heap
	// malloc is neat and all, but it doesn't make use of the 'size' argument provided to deallocate, which means allocated blocks have extra overhead to track that size
	struct malloc_resource 
	{
		// If the memory fails to allocate, the byte span will have a null pointer
		byte_span allocate(size_t byte_size) noexcept 
		{ 
			return { static_cast<std::byte*>(std::malloc(byte_size)), byte_size }; 
		}
		// For malloc_resource, the size of 'bytes' can have any value. 
		void deallocate(byte_span bytes) noexcept 
		{ 
			std::free(bytes.data);
		}
	};
}