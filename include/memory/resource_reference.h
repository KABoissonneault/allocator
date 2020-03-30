#pragma once

#include "memory/byte_span.h"
#include "memory/resource.h"

#include <type_traits>

namespace kab
{
	namespace detail
	{
		template<typename Derived, typename Resource, typename = std::void_t<>>
		struct over_allocate_mixin
		{

		};

		template<typename Derived, typename Resource>
		struct over_allocate_mixin<Derived, Resource
			, std::void_t<decltype(std::declval<Resource>().over_allocate(std::declval<size_t>(), std::declval<align_t>()))>
		>
		{
			[[nodiscard]] byte_span over_allocate(size_t n, align_t alignment)
			{
				return static_cast<Derived&>(*this).m_resource->over_allocate(n, alignment);
			}
		};

		template<typename Derived, typename Resource, typename = std::void_t<>>
		struct over_deallocate_mixin
		{

		};

		template<typename Derived, typename Resource>
		struct over_deallocate_mixin<Derived, Resource
			, std::void_t<decltype(std::declval<Resource>().over_deallocate(std::declval<byte_span>(), std::declval<align_t>()))>
		>
		{
			void over_deallocate(byte_span s, align_t alignment)
			{
				static_cast<Derived&>(*this).m_resource->over_deallocate(s, alignment);
			}
		};
	}
	template<typename Resource>
	class resource_reference :
		public detail::over_allocate_mixin<resource_reference<Resource>, Resource>
		, public detail::over_deallocate_mixin<resource_reference<Resource>, Resource>
	{
		friend struct detail::over_allocate_mixin<resource_reference<Resource>, Resource>;
		friend struct detail::over_deallocate_mixin<resource_reference<Resource>, Resource>;
		
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