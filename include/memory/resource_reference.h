#pragma once

#include "memory/byte_span.h"
#include "memory/resource.h"

#include <type_traits>

namespace kab
{
	template<typename Resource>
	class resource_reference
	{
		Resource* m_resource;

	public:
		resource_reference(Resource& resource)
			: m_resource(&resource)
		{

		}

		[[nodiscard]] byte_span allocate(size_t n, align_t alignment)
		{
			return m_resource->allocate(n, alignment);
		}

		[[nodiscard]] byte_span over_allocate(size_t n, align_t alignment)
		{
			return m_resource->over_allocate(n, alignment);
		}

		void deallocate(byte_span s, align_t alignment) noexcept
		{
			m_resource->deallocate(s, alignment);
		}

		[[nodiscard]] bool operator==(resource_reference rhs) const noexcept
		{
			if constexpr (std::is_empty_v<Resource>)
			{
				return true;
			}
			else
			{
				return m_resource == rhs.m_resource || *m_resource == *rhs.m_resource;
			}
		}
	};
}