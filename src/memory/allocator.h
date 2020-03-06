#pragma once

namespace kab
{
	namespace detail
	{
		template<typename MemoryResource>
		class allocator_storage {
			MemoryResource* resource;
		public:
			allocator_storage(MemoryResource & resource)
				: resource(&resource)
			{

			}

			MemoryResource& get_resource() { return *resource; }
			bool operator==(allocator_storage const& rhs) const noexcept
			{
				return resource == rhs.resource || *resource == *rhs.resource;
			}
		};

		template<typename MemoryResource>
		struct allocator_monostate
		{
			allocator_monostate() = default;
			allocator_monostate(MemoryResource)
			{

			}

			MemoryResource get_resource() { return MemoryResource(); }
			constexpr bool operator==(allocator_monostate const& rhs) const noexcept
			{
				return true;
			}
		};

		template<typename, typename = std::void_t<>>
		struct over_allocate_helper : std::false_type
		{
			template<typename MemoryResource>
			byte_span over_allocate(MemoryResource&& resource, size_t byte_size)
			{
				return resource.allocate(byte_size);
			}
		};

		template<typename T>
		struct over_allocate_helper < T,
			std::void_t<decltype(std::declval<T&>().over_allocate(0))>
		> : std::true_type
		{
			template<typename MemoryResource>
			byte_span over_allocate(MemoryResource&& resource, size_t byte_size)
			{
				return resource.over_allocate(byte_size);
			}
		};
	}

	/**
	 * 'allocator' is a type handling a memory resource.
	 *
	 * This allocator type propagates the memory resource when copying.
	 * If the memory resource type is monostate, the allocator simply creates the memory resource object on demand, no storage used
	 * If the memory resource type is stateful, then the allocator holds a non-owning pointer to the resource, and the user is responsible for keeping the resource alive
	 */
	template<typename MemoryResource, typename PrivateStorage = std::conditional_t<std::is_empty_v<MemoryResource>, detail::allocator_monostate<MemoryResource>, detail::allocator_storage<MemoryResource>>>
	class allocator : PrivateStorage {
		PrivateStorage& access_storage() noexcept { return static_cast<PrivateStorage&>(*this); }
		PrivateStorage const& access_storage() const noexcept { return static_cast<PrivateStorage const&>(*this); }
	public:
		using memory_resource = MemoryResource;

		allocator() = default;
		allocator(MemoryResource & resource)
			: PrivateStorage(resource)
		{

		}

		byte_span allocate(size_t byte_size) { return access_storage().get_resource().allocate(byte_size); }
		byte_span over_allocate(size_t byte_size) { return detail::over_allocate_helper<MemoryResource>{}.over_allocate(access_storage().get_resource(), byte_size); }
		void deallocate(byte_span bytes) noexcept { return access_storage().get_resource().deallocate(bytes); }

		static constexpr bool is_always_equal() noexcept
		{
			return std::is_empty_v<MemoryResource>;
		}
		constexpr bool operator==(allocator const& rhs) const noexcept
		{
			return access_storage() == rhs.access_storage();
		}
	};
}