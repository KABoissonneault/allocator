#pragma once

#include <iterator>
#include <utility>

namespace kab
{
	/**
	 * 'vector' is a dynamically-resizing contiguous container
	 *
	 * Kind of like std::vector, but with my allocator model.
	 * T must be Relocatable.
	 * 'vector' only provides basic exception guarantees for all functions
	 * 'vector' itself is also Relocatable
	 */
	template<typename T, typename Allocator = allocator<malloc_resource>>
	class vector : Allocator {
		Allocator& access_alloc() & noexcept { return static_cast<Allocator&>(*this); }
		Allocator const& access_alloc() const& noexcept { return static_cast<Allocator const&>(*this); }
		Allocator&& access_alloc() && noexcept { return static_cast<Allocator&&>(*this); }

		T* m_data = nullptr; // beginning of "data"
		T* m_size = nullptr; // end of the "size"
		size_t m_byte_capacity = 0;

		static size_t get_next_capacity(size_t current)
		{
			if (current < 4) {
				return 4;
			}
			else {
				return current * 2;
			}
		}

		void free_storage()
		{
			access_alloc().deallocate({ reinterpret_cast<std::byte*>(m_data), m_byte_capacity });
		}

		void reallocate(size_t new_capacity)
		{
			byte_span const new_block = access_alloc().over_allocate(new_capacity * sizeof(T));
			size_t const current_size = size();

			// Relocate data
			memcpy(new_block.data, m_data, current_size * sizeof(T));

			free_storage();

			m_data = reinterpret_cast<T*>(new_block.data);
			m_size = m_data + current_size;
			m_byte_capacity = new_block.size;
		}

		void ensure_capacity(size_t n)
		{
			const size_t current_capacity = capacity();
			if (current_capacity < n)
			{
				size_t new_capacity;
				do
				{
					new_capacity = get_next_capacity(current_capacity);
				} while (new_capacity < n);
				reserve(new_capacity);
			}
		}
	public:
		vector() = default;
		vector(vector && rhs) noexcept
			: Allocator(std::move(rhs).access_alloc())
			, m_data(std::exchange(rhs.m_data, nullptr))
			, m_size(std::exchange(rhs.m_size, nullptr))
			, m_byte_capacity(std::exchange(rhs.m_byte_capacity, 0))
		{

		}
		vector& operator=(vector&& rhs) noexcept
		{
			std::destroy(m_data, m_size);
			free_storage();

			access_alloc() = std::move(rhs).access_alloc();
			m_data = std::exchange(rhs.m_data, nullptr);
			m_size = std::exchange(rhs.m_size, nullptr);
			m_byte_capacity = std::exchange(rhs.m_byte_capacity, 0);

			return *this;
		}
		~vector()
		{
			std::destroy(m_data, m_size);
			free_storage();
		}

		vector(Allocator alloc)
			: Allocator(alloc)
		{

		}

		vector(typename Allocator::memory_resource & resource)
			: Allocator(resource)
		{

		}
		vector(typename Allocator::memory_resource && resource)
			: Allocator(std::move(resource))
		{

		}

		using allocator_type = Allocator;

		/**
		 * If the container uses a propagating allocator, this function returns a copy of the allocator
		 */
		allocator_type get_allocator() const noexcept { return access_alloc(); }

		/**
		 * Factory function creating a vector copying the data from another container and propagating its allocator
		 */
		template<typename Container>
		static vector from_container(Container const& c)
		{
			vector v(c.get_allocator());
			v.insert(c);
			return v;
		}

		T* data() noexcept { return m_data; }
		T const* data() const noexcept { return m_data; }
		size_t size() const noexcept { return std::distance(m_data, m_size); }
		size_t capacity() const noexcept { return m_byte_capacity / sizeof(T); }
		T* begin() noexcept { return m_data; }
		T* end() noexcept { return m_size; }
		T const* begin() const noexcept { return m_data; }
		T const* end() const noexcept { return m_size; }

		T & operator[](size_t i) { return m_data[i]; }
		T const& operator[](size_t i) const { return m_data[i]; }

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
			if (capacity() >= n)
			{
				return;
			}

			reallocate(n);
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
}