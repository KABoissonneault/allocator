#pragma once

#include <memory/byte_span.h>

#include <cstdlib>
#include <new>

class test_resource
{
	size_t m_last_alloc = 0;
	size_t m_last_align = 0;
	size_t m_current_alloc = 0;
	size_t m_total_alloc = 0;
public:
	test_resource() = default;
	test_resource(test_resource const&) = delete;
	test_resource& operator=(test_resource const&) = delete;

	[[nodiscard]] kab::byte_span allocate(size_t n, kab::align_t alignment)
	{
		m_last_alloc = n;
		m_last_align = static_cast<size_t>(alignment);
		m_current_alloc += n;
		m_total_alloc += n;
		void* const ptr = ::operator new(n, static_cast<std::align_val_t>(alignment));
		return { static_cast<kab::byte*>(ptr), n };
	}

	void deallocate(kab::byte_span s, kab::align_t alignment)
	{
		m_current_alloc -= s.size;
		::operator delete(s.data, s.size, static_cast<std::align_val_t>(alignment));
	}

	// Get the size of the last call to 'allocate'
	size_t get_last_alloc() const noexcept { return m_last_alloc; }
	// Get the alignment of the last call to 'allocate'
	size_t get_last_align() const noexcept { return m_last_align; }
	// Get the total number of bytes currently allocated
	size_t get_current_alloc() const noexcept { return m_current_alloc; }
	// Get the total number of bytes ever allocated
	size_t get_total_alloc() const noexcept { return m_total_alloc; }

	[[nodiscard]] friend bool operator==(test_resource const& lhs, test_resource const& rhs) noexcept
	{
		return &lhs == &rhs;
	}
};
