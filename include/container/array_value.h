#pragma once

namespace kab
{
	/**
	 * array_value is a shared immutable container of a contiguous range of elements
	 *
	 * On copy, the memory resource is propagated, and the array value is shared between the source and destination containers.
	 * Copying from an array_value object requires no external synchronization with other 'const' operations on the array_value.
	 * However, mutating an array_value object by changing its value does require external synchronization with other operations on the same object
	 *
	 * Due to the possibility of "sub-arrays" sharing ownership, the element range of an array_value may not exactly correspond to the owned value
	 * 
	 */
	template<typename ElementT, typename ResourceT>
	class array_value : ResourceT
	{
		[[nodiscard]] ResourceT& access_resource() & noexcept { return static_cast<ResourceT&>(*this); }
		[[nodiscard]] ResourceT const& access_resource() const& noexcept { return static_cast<ResourceT const&>(*this); }
		[[nodiscard]] ResourceT&& access_resource() && noexcept { return static_cast<ResourceT&&>(*this); }

		struct control 
		{
			size_t count;
			size_t size; // number of elements 
			ElementT fam[1]; // actually a FAM
		};

		// allocates and construct the control, but not the elements
		// the control comes with extra space for (size-1) elements contiguously after 'fam'
		// the control starts with a count of 1
		static control* new_control(ResourceT& r, size_t size);
		// increase the count of the control
		static control* acquire_control(control* c) noexcept;
		// decrease the count of the control, and deletes it if last
		static void release_control(ResourceT& r, control* c);

		control* m_control = nullptr;
		ElementT* m_data = nullptr;
		ElementT* m_end = nullptr;

	public:
		array_value() = default;
		explicit array_value(ResourceT r) noexcept;
		array_value(array_value const& rhs) noexcept;
		array_value(array_value && rhs) noexcept;
		array_value& operator=(array_value const& rhs) noexcept;
		array_value& operator=(array_value && rhs) noexcept;
		~array_value();
		void swap(array_value& rhs) noexcept;

		using value_type = ElementT;
		using memory_resource = ResourceT;
		using iterator = value_type*;
		using const_iterator = value_type const*;

		/**
		 * Get a copy of the resource's value
		 */
		[[nodiscard]] memory_resource get_resource() const noexcept { return access_resource(); }

		/**
		 * Assigns a new value to the container.
		 *
		 * Requires: SizedRanged is a range (begin / end) and is sized (size)
		 */
		template<typename SizedRangeT>
		array_value& assign(SizedRangeT&& r);

		[[nodiscard]] iterator begin() noexcept { return m_data; }
		[[nodiscard]] const_iterator begin() const noexcept { return m_data; }
		[[nodiscard]] iterator end() noexcept { return m_end; }
		[[nodiscard]] const_iterator end() const noexcept { return m_end; }
		[[nodiscard]] value_type* data() noexcept { return m_data; }
		[[nodiscard]] value_type const* data() const noexcept { return m_data; }
		[[nodiscard]] size_t size() const noexcept { return m_end - m_data; }
		[[nodiscard]] bool is_empty() const noexcept { return m_data == m_end; }
		[[nodiscard]] value_type& front() { return *m_data; }
		[[nodiscard]] value_type const& front() const { return *m_data; }
		[[nodiscard]] value_type& back() { return *(m_end - 1); }
		[[nodiscard]] value_type const& back() const { return *(m_end - 1); }
	};
}

#include "container/inl/array_value.inl.h"