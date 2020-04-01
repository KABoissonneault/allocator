#pragma once

namespace kab
{
	template<typename T, typename R>
	constexpr size_t vector<T, R>::get_next_capacity(size_t current) noexcept
	{
		if (current < 4) {
			return 4;
		}
		else {
			return current * 2;
		}
	}

	template<typename T, typename R>
	void vector<T, R>::free_storage() noexcept
	{
		detail::over_deallocate(access_resource(), { reinterpret_cast<byte*>(m_data), m_byte_capacity }, static_cast<align_t>(alignof(T)));
	}

	template<typename T, typename R>
	void vector<T, R>::reallocate(size_t new_capacity)
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

	template<typename T, typename R>
	void vector<T, R>::ensure_capacity(size_t n)
	{
		const size_t current_capacity = capacity();
		if (current_capacity < n)
		{
			const size_t new_capacity = get_next_capacity(current_capacity);
			reallocate(std::max(new_capacity, n));
		}
	}
	
	template<typename T, typename R>
	vector<T, R>::vector(vector && rhs) noexcept
		: R(std::move(rhs).access_resource())
		, m_data(std::exchange(rhs.m_data, nullptr))
		, m_size(std::exchange(rhs.m_size, nullptr))
		, m_byte_capacity(std::exchange(rhs.m_byte_capacity, 0))
	{

	}

	template<typename T, typename R>
	auto vector<T, R>::operator=(vector && rhs) noexcept -> vector&
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

	template<typename T, typename R>
	vector<T, R>::~vector()
	{
		std::destroy(m_data, m_size);
		free_storage();
	}

	template<typename T, typename R>
	void vector<T, R>::swap(vector& rhs) noexcept
	{
		using std::swap;
		swap(access_resource(), rhs.access_resource());
		swap(m_data, rhs.m_data);
		swap(m_size, rhs.m_size);
		swap(m_byte_capacity, rhs.m_byte_capacity);
	}

	template<typename T, typename R>
	template<typename Range>
	auto vector<T, R>::assign(Range&& r) -> vector&
	{
		std::destroy(m_data, m_size);
		free_storage();
		m_data = nullptr;
		m_size = nullptr;
		m_byte_capacity = 0;
		insert_back(std::forward<Range>(r));
	}

	template<typename T, typename R>
	template<typename Container>
	auto vector<T, R>::from_container(Container const& c) -> vector
	{
		vector v(c.get_resource());
		v.insert_back(c);
		return v;
	}

	template<typename T, typename R>
	constexpr size_t vector<T, R>::max_capacity() noexcept
	{ 
		// TODO: consider 'max_capacity' of the memory resource if available
		return size_t_max_v / sizeof(T); 
	}
	
	template<typename T, typename R>
	auto vector<T, R>::push_back() -> vector&
	{
		ensure_capacity(size() + 1);
		new(m_size) T;
		++m_size;

		return *this;
	}

	template<typename T, typename R>
	auto vector<T, R>::push_back_n(size_t n) -> vector&
	{
		ensure_capacity(size() + n);
		T const* sent = m_size + n;
		for (; m_size != sent; ++m_size) {
			new(m_size) T;
		}

		return *this;
	}

	template<typename T, typename R>
	auto vector<T, R>::push_back(T const& e) -> vector&
	{
		ensure_capacity(size() + 1);
		new(m_size) T(e);
		++m_size;

		return *this;
	}

	template<typename T, typename R>
	auto vector<T, R>::push_back(T && e) -> vector&
	{
		ensure_capacity(size() + 1);
		new(m_size) T(std::move(e));
		++m_size;

		return *this;
	}

	template<typename T, typename R>
	template<typename... Args>
	auto vector<T, R>::emplace_back(Args&&... args) -> vector&
	{
		ensure_capacity(size() + 1);
		new(m_size) T(std::forward<Args>(args)...);
		++m_size;

		return *this;
	}

	template<typename T, typename R>
	auto vector<T, R>::pop_back() -> vector&
	{
		std::destroy_at(--m_size);
		return *this;
	}

	template<typename T, typename R>
	template<typename Range>
	auto vector<T, R>::insert_back(Range&& r) -> vector&
	{
		using std::begin;
		using std::end;

		auto it = begin(r);
		auto const sent = end(r);
		// TODO: reserve if 'Range' is a sized range or 'it' is a RandomAccessIterator 
		for (; it != sent; ++it) {
			emplace_back(*it);
		}

		return *this;
	}

	template<typename T, typename R>
	void vector<T, R>::reserve(size_t n)
	{
		if (capacity() < n) {
			reallocate(n);
		}
	}

	template<typename T, typename R>
	void vector<T, R>::resize(size_t n)
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
		}
		else if (current_size > n) {
			m_size = data() + n;
			std::destroy(m_size, m_size + (current_size - n));
		}
	}

	template<typename T, typename R>
	void vector<T, R>::clear() noexcept
	{
		std::destroy(m_data, m_size);
		m_size = m_data;
	}

	template<typename T, typename R>
	void vector<T, R>::clear_and_shrink() noexcept
	{
		std::destroy(m_data, m_size);
		detail::over_deallocate(access_resource(), { reinterpret_cast<byte*>(m_data), m_byte_capacity }, static_cast<align_t>(alignof(T)));
		m_data = nullptr;
		m_size = nullptr;
		m_byte_capacity = 0;
	}

	template<typename T, typename R>
	void vector<T, R>::shrink_to_fit()
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
}