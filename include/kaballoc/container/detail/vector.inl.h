#pragma once

#include "kaballoc/memory/detail/over_allocate.h"
#include "kaballoc/memory/detail/uninitialized_relocate.h"
#include "kaballoc/core/comparison.h"
#include <utility>

namespace kab
{
	template<typename T, typename R>
	void vector<T, R>::free_storage() noexcept
	{
		detail::over_deallocate(access_resource(), { reinterpret_cast<byte*>(m_data), m_byte_capacity }, align_v<T>);
	}

	template<typename T, typename R>
	void vector<T, R>::reallocate(size_t new_capacity)
	{
		byte_span const new_block = detail::over_allocate(access_resource(), new_capacity * sizeof(T), align_v<T>);
		size_t const current_size = size();

		auto const new_buffer = reinterpret_cast<T*>(new_block.data);

		// Relocate data
		kab::uninitialized_relocate(m_data, m_size, new_buffer);

		// Free the previous storage
		free_storage();

		// Use the new storage
		m_data = new_buffer;
		m_size = m_data + current_size;
		m_byte_capacity = new_block.size;
	}

	template<typename T, typename R>
	void vector<T, R>::ensure_capacity(size_t n)
	{
		const size_t current_capacity = capacity();
		if (current_capacity < n)
		{
			reallocate(n);
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
			kab::destroy(m_data, m_size);
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
		kab::destroy(m_data, m_size);
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
	constexpr size_t vector<T, R>::max_capacity() noexcept
	{ 
		// TODO: consider 'max_capacity' of the memory resource if available
		return size_t_max_v / sizeof(T); 
	}
	
	template<typename T, typename R>
	auto vector<T, R>::push_back() -> T &
	{
		ensure_capacity(size() + 1);
		T* ptr = new(m_size) T;
		++m_size;

		return *ptr;
	}

	template<typename T, typename R>
	void vector<T, R>::push_back_n(size_t n)
	{
		ensure_capacity(size() + n);
		T const* sent = m_size + n;
		for (; m_size != sent; ++m_size) {
			new(m_size) T;
		}
	}

	template<typename T, typename R>
	auto vector<T, R>::push_back(T const& e) -> T &
	{
		ensure_capacity(size() + 1);
		T* ptr = new(m_size) T(e);
		++m_size;

		return *ptr;
	}

	template<typename T, typename R>
	auto vector<T, R>::push_back(T && e) -> T &
	{
		ensure_capacity(size() + 1);
		T* ptr = new(m_size) T(std::move(e));
		++m_size;

		return *ptr;
	}

	template<typename T, typename R>
	void vector<T, R>::pop_back()
	{
		kab::destroy_at(--m_size);
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
			kab::destroy(m_size, m_size + (current_size - n));
		}
	}

	template<typename T, typename R>
	void vector<T, R>::clear() noexcept
	{
		kab::destroy(m_data, m_size);
		m_size = m_data;
	}

	template<typename T, typename R>
	void vector<T, R>::clear_and_shrink() noexcept
	{
		kab::destroy(m_data, m_size);
		detail::over_deallocate(access_resource(), { reinterpret_cast<byte*>(m_data), m_byte_capacity }, align_v<T>);
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

		reallocate(current_size);
	}
}
