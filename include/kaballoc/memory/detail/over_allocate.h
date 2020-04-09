#pragma once

#include <type_traits>

#include "kaballoc/memory/resource.h"
#include "kaballoc/memory/byte_span.h"

namespace kab::detail
{
	template<typename, typename = std::void_t<>>
	struct over_allocate_helper : std::false_type
	{
		template<typename MemoryResource>
		byte_span over_allocate(MemoryResource&& resource, size_t byte_size, align_t align)
		{
			return resource.allocate(byte_size, align);
		}
	};

	template<typename T>
	struct over_allocate_helper < T,
		std::void_t<decltype(std::declval<T&>().over_allocate(0, std::declval<align_t>()))>
	> : std::true_type
	{
		template<typename MemoryResource>
		byte_span over_allocate(MemoryResource&& resource, size_t byte_size, align_t align)
		{
			return resource.over_allocate(byte_size, align);
		}
	};

	template<typename, typename = std::void_t<>>
	struct over_deallocate_helper : std::false_type
	{
		template<typename MemoryResource>
		void over_deallocate(MemoryResource&& resource, byte_span s, align_t align)
		{
			resource.deallocate(s, align);
		}
	};

	template<typename T>
	struct over_deallocate_helper < T,
		std::void_t<decltype(std::declval<T&>().over_deallocate(std::declval<byte_span>(), std::declval<align_t>()))>
	> : std::true_type
	{
		template<typename MemoryResource>
		void over_deallocate(MemoryResource&& resource, byte_span s, align_t align)
		{
			resource.over_deallocate(s, align);
		}
	};

	template<typename MemoryResource>
	inline byte_span over_allocate(MemoryResource&& resource, size_t byte_size, align_t align)
	{
		return over_allocate_helper<MemoryResource>().over_allocate(std::forward<MemoryResource>(resource), byte_size, align);
	}

	template<typename MemoryResource>
	inline void over_deallocate(MemoryResource&& resource, byte_span s, align_t align)
	{
		over_deallocate_helper<MemoryResource>().over_deallocate(std::forward<MemoryResource>(resource), s, align);
	}
}