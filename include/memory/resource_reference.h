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

		[[nodiscard]] friend bool operator==(resource_reference lhs, resource_reference rhs) noexcept
		{
			if constexpr (std::is_empty_v<Resource>)
			{
				return true;
			}
			else
			{
				return lhs.m_resource == rhs.m_resource || *lhs.m_resource == *rhs.m_resource;
			}
		}
	};

	template<typename Resource>
	auto make_reference(Resource& resource) noexcept -> kab::resource_reference<std::remove_const_t<std::remove_reference_t<Resource>>>
	{
		return { resource };
	}
}