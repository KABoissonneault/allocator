#include "memory/new_resource.h"

#include "new.h"

#include <catch.hpp>

TEST_CASE("New resource", "[memory]")
{
	class new_tester : public new_observer
	{
		kab::size_t m_last_alloc{ 0 };
		kab::align_t m_last_align{ 0 };
		kab::size_t m_current_alloc{ 0 };
		kab::size_t m_total_alloc{ 0 };

	public:
		void on_new(std::size_t size, std::align_val_t alignment) override
		{
			m_last_alloc = size;
			m_last_align = static_cast<kab::align_t>(alignment);
			m_current_alloc += size;
			m_total_alloc += size;
		}

		void on_delete(void* ptr, std::size_t size, std::align_val_t alignment) override
		{
			(void)ptr;
			(void)alignment;
			m_current_alloc -= size;
		}

		kab::size_t get_last_alloc() const noexcept { return m_last_alloc; }
		kab::align_t get_last_alignment() const noexcept { return m_last_align; }
		kab::size_t get_current_alloc() const noexcept { return m_current_alloc; }
		kab::size_t get_total_alloc() const noexcept { return m_total_alloc; }
	};

	new_tester tester;
	set_new_observer(tester);

	kab::new_resource resource;
	kab::byte_span const s = resource.allocate(16, kab::max_align_v);
	REQUIRE(tester.get_last_alloc() == 16);
	REQUIRE(tester.get_last_alignment() == kab::max_align_v);
	REQUIRE(tester.get_current_alloc() == 16);
	REQUIRE(tester.get_total_alloc() == 16);

	resource.deallocate(s, kab::max_align_v);
	REQUIRE(tester.get_current_alloc() == 0);
}