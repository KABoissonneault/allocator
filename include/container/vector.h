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
		[[nodiscard]] MemoryResource& access_resource() & noexcept { return static_cast<MemoryResource&>(*this); }
		[[nodiscard]] MemoryResource const& access_resource() const& noexcept { return static_cast<MemoryResource const&>(*this); }
		[[nodiscard]] MemoryResource&& access_resource() && noexcept { return static_cast<MemoryResource&&>(*this); }

		T* m_data = nullptr; // beginning of "data"
		T* m_size = nullptr; // end of the "size"
		size_t m_byte_capacity = 0;

		[[nodiscard]] static constexpr size_t get_next_capacity(size_t current) noexcept;
		void free_storage() noexcept;
		void reallocate(size_t new_capacity);
		void ensure_capacity(size_t n);
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
		vector(vector && rhs) noexcept;
		/**
		 * vector is never copy assignable
		 */
		vector& operator=(vector const& rhs) = delete;
		/**
		 * vector is move assignable if the memory resource is moveable
		 */
		vector& operator=(vector && rhs) noexcept;

		/**
		 * Destroys all the elements of the vector, frees the storage, and destroys the memory resource
		 */
		~vector();

		/**
		 * vector is swappable if the memory resource is swappable
		 */
		friend void swap(vector& v1, vector& v2) noexcept;

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
		[[nodiscard]] memory_resource get_resource() const noexcept { return access_resource(); }

		/**
		 * Replaces the current element range of the vector with the one from the input range
		 *
		 * Requires: 
		 *   - Range is a Range
		 *   - T is constructible from the element type of Range
		 */
		template<typename Range>
		vector& assign(Range&& r);

		/**
		 * Factory function creating a vector using another container, copying its memory resource and copying its element range.
		 *
		 * Requires: 
		 *   - Container is a valid ResourceAwareContainer
		 *   - The element type of Container can construct T
		 */
		template<typename Container>
		[[nodiscard]] static vector from_container(Container const& c);

		/**
		 * Return a pointer to the start of the constructed elements.
		 *
		 * If the vector is empty, this will still return a valid but non-dereferenceable pointer.
		 * If called with const access, this will return a const pointer.
		 */
		[[nodiscard]] T* data() noexcept { return m_data; }
		[[nodiscard]] T const* data() const noexcept { return m_data; }
		
		/**
		 * Returns whether the vector has no elements
		 *
		 * Note that the capacity may not necessarily be zero if this is true
		 */
		[[nodiscard]] bool is_empty() const noexcept { return m_data == m_size; }

		/**
		 * Returns the size of the vector.
		 * 
		 * This represents the number of constructed elements.
		 */
		[[nodiscard]] size_t size() const noexcept { return std::distance(m_data, m_size); }
		
		/**
		 * Returns the capacity of the vector.
		 *
		 * As long as the resulting size is smaller or equal to this capacity, constructing functions will not allocate
		 */
		[[nodiscard]] size_t capacity() const noexcept { return m_byte_capacity / sizeof(T); }

		/**
		 * Returns the maximum possible capacity for the current vector type.
		 *
		 * If the memory resource type also has a maximum capacity, that value is taken into account.
		 */
		[[nodiscard]] static constexpr size_t max_capacity() noexcept;
		
		/**
		 * 'begin' and 'end' return iterators to the element range of the vector
		 *
		 * If called with const access, this will return const iterators 
		 */
		[[nodiscard]] iterator begin() noexcept { return m_data; }
		[[nodiscard]] sentinel end() noexcept { return m_size; }
		[[nodiscard]] const_iterator begin() const noexcept { return m_data; }
		[[nodiscard]] const_sentinel end() const noexcept { return m_size; }
		
		/**
		 * Returns a reference to the first element of the vector
		 *
		 * Precondition: The size must be at least 1
		 */
		[[nodiscard]] T& front() { return *m_data; }

		/**
		 * Returns a reference to the last element of the vector.
		 *
		 * Precondition: The size must be at least 1
		 */
		[[nodiscard]] T& back() { return *(m_size - 1); }

		/**
		 * Operator[]. Accesses elements of the vector using a zero-based index, returning a reference to the specified element.
		 *
		 * Precondition: 'i' must be smaller than the size
		 */
		[[nodiscard]] T & operator[](size_t i) { return m_data[i]; }
		[[nodiscard]] T const& operator[](size_t i) const { return m_data[i]; }

		/**
		 * Construct a new default-initialized element at the back of the vector
		 *
		 * Requires: T is DefaultConstructible
		 */
		vector& push_back();

		/**
		 * Constructs 'n' new default-initialized elements at the back of the vector
		 *
		 * Requires: T is DefaultConstructible
		 */
		vector& push_back_n(size_t n);

		/**
		 * Constructs a new element at the back of the vector by copying the provided argument
		 *
		 * Requires: T is CopyConstructible
		 */
		vector& push_back(T const& e);
		
		/**
		 * Constructs a new element at the back of the vector by moving the provided argument
		 * 
		 * Requires: T must be MoveConstructible
		 */
		vector& push_back(T && e);
		
		/**
		 * Constructs a new element at the back of the vector from the provided arguments
		 *
		 * Requires: 'T' must be constructible from the provided arguments
		 */
		template<typename... Args>
		vector& emplace_back(Args &&... args);

		/**
		 * Removes the last element of the vector.
		 * 
		 * Precondition: The size of the vector must be at least 1
		 */
		vector& pop_back();

		/**
		 * Inserts an entire Range at the back of the vector
		 *
		 * Requires: T must be constructible from the element type of Range
		 */
		template<typename Range>
		vector& insert_back(Range&& r);

		/**
		 * Changes the capacity of the vector, without changing the size of the vector
		 */
		void reserve(size_t n);

		/**
		 * Changes the size of the vector, ensuring proper capacity and constructing or destroying elements as necessary
		 * New elements are default-initialized
		 */
		void resize(size_t n);

		/**
		 * Removes all elements from the vector, making its size 0
		 * Does not free the storage.
		 */
		void clear() noexcept;

		/**
		 * Removes all elements from the vector, making its size 0, then frees the storage.
		 * Unlike calling 'clear' and 'shrink_to_fit' in a sequence, this function is 'noexcept', since it never allocates
		 */
		void clear_and_shrink() noexcept;

		/**
		 * Potentially reallocate to reduce the capacity of the vector to match the size as much as possible
		 * Note that the vector will still over-allocate, and as a result may still end up with a capacity greater than the size
		 * If the size of the vector is 0, the current storage is freed without allocating a new one
		 */
		void shrink_to_fit();
	};

	template<typename T, typename MemoryResource>
	struct is_trivially_relocatable<vector<T, MemoryResource>> 
		: std::conditional_t<std::is_empty_v<MemoryResource> || is_trivially_relocatable_v<MemoryResource>, std::true_type, std::false_type>
	{

	};
}

#include "container/inl/vector­.inl.h"