#pragma once

#include "memory/byte_span.h"

namespace kab
{
	/**
	 * 'monotonic_resource' is a simple resource wrapper that allocates a buffer from an inner resource, and on allocation returns parts of that buffer, moving mostly upwards
	 * On deallocation, this resource only "frees" the memory if it's at the top of the buffer. Otherwise, memory is only freed when the monotonic resource itself is cleared.
	 */
	template<typename InnerResource>
	class monotonic_resource : InnerResource
	{
		InnerResource& access_inner() & noexcept { return static_cast<InnerResource&>(*this); }
		InnerResource&& access_inner() && noexcept { return static_cast<InnerResource&&>(*this); }

		struct header
		{
			header* next;
			size_t size;
			byte data[1]; // this is actually a FAM - we use this to get the address of the actual buffer
		};

		header* head = nullptr;

	public:
		monotonic_resource() = default;
		monotonic_resource(InnerResource resource)
			: InnerResource(std::move(resource))
		{

		}

		// Adds a "buffer" to the monotonic resource, ensuring future allocations up to that size come from that buffer
		void add_buffer(size_t n)
		{
			const byte_span buffer = access_inner().allocate(n);
		}


	};
}