#pragma once

#include <iterator>
#include <utility>

#include "trait/relocatable.h"
#include "memory/detail/over_allocate.h"

namespace kab
{
	/**
	 * 'vector' is a dynamically-resizing contiguous container
	 *
	 * Kind of like std::vector, but with the kab allocator model. 
	 *
	 * The MemoryResource needs to match the kab::memory_resource concept.
	 * If the MemoryResource is an over-allocator, the vector will use the over-allocation functions.
	 *
	 * vector is never copyable, is noexcept moveable if the resource is moveable, and is trivially relocatable if the resource is relocatable or empty
	 * 
	 * Functions that add elements to the container can cause a reallocation if the size cannot grow beyond the current capacity.
	 * On any function that can reallocate, vector requires its element type to be trivially relocatable. 
	 * Some of these functions may additionally require moveability or copyability
	 */
	template<typename T, typename MemoryResource>
	class vector : MemoryResource {
		MemoryResource& access_resource() & noexcept { return static_cast<MemoryResource&>(*this); }
		MemoryResource const& access_resource() const& noexcept { return static_cast<MemoryResource const&>(*this); }
		MemoryResource&& access_resource() && noexcept { return static_cast<MemoryResource&&>(*this); }

		T* m_data = nullptr; // beginning of "data"
		T* m_size = nullptr; // end of the "size"
		size_t m_byte_capacity = 0;

		static constexpr size_t get_next_capacity(size_t current) noexcept
		{
			if (current < 4) {
				return 4;
			} else {
				return current * 2;
			}
		}

		void free_storage() noexcept
		{
			detail::over_deallocate(access_resource(), { reinterpret_cast<byte*>(m_data), m_byte_capacity }, static_cast<align_t>(alignof(T)));
		}

		void reallocate(size_t new_capacity)
		{
			byte_span const new_block = detail::over_allocate(access_resource(), new_capacity * sizeof(T), static_cast<align_t>(alignof(T)));
			size_t const current_size = size();

			// Default construct the new objects
			T* new_it = new_block.data;
			T* const new_sent = new_it + current_size;
			for (; new_it != new_sent; ++new_it) {
				new(new_it) T;
			}

			// Relocate data
			memcpy(new_block.data, m_data, current_size * sizeof(T));

			// Free the previous storage
			free_storage();

			// Use the new storage
			m_data = reinterpret_cast<T*>(new_block.data);
			m_size = m_data + current_size;
			m_byte_capacity = new_block.size;
		}

		void ensure_capacity(size_t n)
		{
			const size_t current_capacity = capacity();
			if (current_capacity < n)
			{
				const size_t new_capacity = get_next_capacity(current_capacity);
				reallocate(std::max(new_capacity, n));
			}
		}
	public:
		/**
		 * vector is default constructible if the memory resource is default constructible
		 */
		vector() = default;
		/**
		 * vector is never copy constructible
		 */
		vector(vector const&) = delete;
		/**
		 * vector is noexcept move constructible if the memory resource is moveable
		 */
		vector(vector && rhs) noexcept
			: MemoryResource(std::move(rhs).access_resource())
			, m_data(std::exchange(rhs.m_data, nullptr))
			, m_size(std::exchange(rhs.m_size, nullptr))
			, m_byte_capacity(std::exchange(rhs.m_byte_capacity, 0))
		{

		}
		/**
		 * vector is never copy assignable
		 */
		vector& operator=(vector const& rhs) = delete;
		/**
		 * vector is move assignable if the memory resource is moveable
		 */
		vector& operator=(vector && rhs) noexcept
		{
			if (this != &rhs)
			{
				std::destroy(m_data, m_size);
				free_storage();

				access_resource() = std::move(rhs).access_resource();
				m_data = std::exchange(rhs.m_data, nullptr);
				m_size = std::exchange(rhs.m_size, nullptr);
				m_byte_capacity = std::exchange(rhs.m_byte_capacity, 0);
			}

			return *this;
		}
		~vector()
		{
			std::destroy(m_data, m_size);
			free_storage();
		}

		/**
		 * vector is swappable if the memory resource is swappable
		 */
		friend void swap(vector& v1, vector& v2) noexcept
		{
			using std::swap;
			swap(v1.access_resource(), v2.access_resource());
			swap(v1.m_data, v2.m_data);
			swap(v1.m_size, v2.m_size);
			swap(v1.m_byte_capacity, v2.m_byte_capacity);
		}

		using value_type = T;
		using memory_resource = MemoryResource;
		using iterator = T*;
		using const_iterator = T const*;
		using sentinel = iterator;
		using const_sentinel = const_iterator;

		/**
		 * If the memory resource is moveable, this constructor lets the user provide a resource value
		 */
		vector(memory_resource r) noexcept
			: MemoryResource(std::move(r))
		{

		}

		/**
		 * Returns the memory resource value used in this container
		 */
		memory_resource get_resource() const noexcept { return access_resource(); }

		/**
		 * Factory function creating a vector copying the data from another container and propagating its allocator
		 */
		template<typename Container>
		static vector from_container(Container const& c)
		{
			vector v(c.get_resource());
			v.insert(c);
			return v;
		}

		T* data() noexcept { return m_data; }
		T const* data() const noexcept { return m_data; }
		size_t size() const noexcept { return std::distance(m_data, m_size); }
		size_t capacity() const noexcept { return m_byte_capacity / sizeof(T); }
		iterator begin() noexcept { return m_data; }
		sentinel end() noexcept { return m_size; }
		const_iterator begin() const noexcept { return m_data; }
		const_sentinel end() const noexcept { return m_size; }
		T& front() { return *m_data; }
		T& back() { return *(m_size - 1); }

		T & operator[](size_t i) { return m_data[i]; }
		T const& operator[](size_t i) const { return m_data[i]; }

		vector& push_back()
		{
			ensure_capacity(size() + 1);
			new(m_size) T;
			++m_size;

			return *this;
		}

		vector& push_back_n(size_t n)
		{
			ensure_capacity(size() + n);
			T const* sent = m_size + n;
			for (; m_size != sent; ++m_size) {
				new(m_size) T;
			}

			return *this;
		}

		vector& push_back(T const& e)
		{
			ensure_capacity(size() + 1);
			new(m_size) T(e);
			++m_size;

			return *this;
		}
		vector& push_back(T && e)
		{
			ensure_capacity(size() + 1);
			new(m_size) T(std::move(e));
			++m_size;

			return *this;
		}
		template<typename... Args>
		vector& emplace_back(Args&&... args)
		{
			ensure_capacity(size() + 1);
			new(m_size) T(std::forward<Args>(args)...);
			++m_size;

			return *this;
		}

		vector& pop_back() noexcept
		{
			std::destroy_at(m_size - 1);
			--m_size;
			return *this;
		}

		template<typename Range>
		vector& insert(Range&& r)
		{
			using std::begin;
			using std::end;

			auto it = begin(r);
			auto const sent = end(r);
			for (; it != sent; ++it) {
				emplace_back(*it);
			}

			return *this;
		}

		void reserve(size_t n)
		{
			if (capacity() < n) {
				reallocate(n);
			}
		}

		void resize(size_t n)
		{
			const size_t current_size = size();
			
			if (current_size < n) {
				if (capacity() < n) {
					reallocate(n);
				}

				// Construct the new objects
				T* it = data() + current_size;
				T* const sent = it + (n - current_size);
				for (; it != sent; ++it) {
					new(it) T;
				}

				// Use the new "size" sentinel
				m_size = it;
			} else if (current_size > n) {
				m_size = data() + n;
				std::destroy(m_size, m_size + (current_size - n));
			}
		}

		void clear() noexcept
		{
			std::destroy(m_data, m_size);
			m_size = m_data;
		}

		void shrink_to_fit()
		{
			size_t const current_size = size();
			size_t const current_capacity = capacity();
			if (current_capacity == current_size)
			{
				return;
			}

			if (current_size == 0)
			{
				free_storage();
				m_data = nullptr;
				m_size = nullptr;
				m_byte_capacity = 0;
				return;
			}

			size_t const target_capacity = get_next_capacity(size());
			if (current_capacity <= target_capacity)
			{
				return;
			}

			reallocate(target_capacity);
		}
	};

	template<typename T, typename MemoryResource>
	struct is_trivially_relocatable<vector<T, MemoryResource>> 
		: std::conditional_t<std::is_empty_v<MemoryResource> || is_trivially_relocatable_v<MemoryResource>, std::true_type, std::false_type>
	{

	};
}