#pragma once

#include "memory/byte_span.h"

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

		[[nodiscard]] byte_span allocate(size_t n)
		{
			return m_resource->allocate(n);
		}

		[[nodiscard]] byte_span over_allocate(size_t n)
		{
			return m_resource->over_allocate(n);
		}

		void deallocate(byte_span s)
		{
			m_resource->deallocate(s);
		}
	};
}