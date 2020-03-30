#include "container/vector.h"

#include <catch.hpp>

#include "memory/resource_reference.h"
#include "memory/new_resource.h"
#include "test_resource.h"

template<typename T>
using vector = kab::vector<T, kab::resource_reference<test_resource>>;

TEST_CASE("Container Vector Compilation", "[container]")
{
	REQUIRE(!std::is_default_constructible_v<vector<int>>); // kab::resource_reference is not default constructible
	REQUIRE(std::is_default_constructible_v<kab::vector<int, kab::new_resource>>);
	REQUIRE(!std::is_copy_constructible_v<vector<int>>);
	REQUIRE(!std::is_copy_assignable_v<vector<int>>);
	REQUIRE(std::is_nothrow_move_constructible_v<vector<int>>);
	REQUIRE(std::is_nothrow_move_assignable_v<vector<int>>);
	REQUIRE(std::is_nothrow_constructible_v<vector<int>, test_resource&>);
	REQUIRE(std::is_nothrow_swappable_v<vector<int>>);
}

TEST_CASE("Container Vector Empty", "[container]")
{
	test_resource resource;
		
	// Default construction
	vector<int> test_vec1(resource);
	REQUIRE(test_vec1.get_resource() == kab::make_reference(resource));
	REQUIRE(test_vec1.size() == 0);
	REQUIRE(test_vec1.capacity() == 0);
	REQUIRE(resource.get_last_alloc() == 0);
	REQUIRE(test_vec1.begin() == test_vec1.end());

	// Move of default value
	vector<int> test_vec2(std::move(test_vec1));
	REQUIRE(test_vec2.get_resource() == kab::make_reference(resource));
	REQUIRE(test_vec2.size() == 0);
	REQUIRE(test_vec2.capacity() == 0);
	REQUIRE(resource.get_last_alloc() == 0);
	REQUIRE(test_vec2.begin() == test_vec1.end());

	// Move of default value
	test_vec1 = std::move(test_vec2);
	REQUIRE(test_vec1.get_resource() == kab::make_reference(resource));
	REQUIRE(test_vec1.size() == 0);
	REQUIRE(test_vec1.capacity() == 0);
	REQUIRE(resource.get_last_alloc() == 0);
	REQUIRE(test_vec1.begin() == test_vec1.end());
	
	// Self-assignment of default value
	test_vec1 = std::move(test_vec1);
	REQUIRE(test_vec1.get_resource() == kab::make_reference(resource));
	REQUIRE(test_vec1.size() == 0);
	REQUIRE(test_vec1.capacity() == 0);
	REQUIRE(resource.get_last_alloc() == 0);
	REQUIRE(test_vec1.begin() == test_vec1.end());
}

TEST_CASE("Container Vector Trivial Value", "[container]")
{
	test_resource r;

	vector<kab::byte> v(r);
	v.reserve(32);
	auto const initial_alloc = r.get_total_alloc();

	for (kab::byte i = 0; i < 32; ++i) {
		v.push_back(i);
	}

	REQUIRE(r.get_total_alloc() == initial_alloc); // the push_backs should not have changed capacity
	REQUIRE(v.size() == 32);
	REQUIRE(v.capacity() >= 32);
	REQUIRE(*v.data() == 0);
	REQUIRE(v[0] == 0);
	REQUIRE(*v.begin() == 0);
	REQUIRE(v.begin() != v.end());
	REQUIRE(v[12] == 12);
	REQUIRE(v[31] == 31);

	v.push_back(32);

	REQUIRE(r.get_total_alloc() > initial_alloc); // expected a realloc
	REQUIRE(v.size() == 33);
	REQUIRE(v.capacity() > 32);
	REQUIRE(v.begin() != v.end());
	REQUIRE(v[32] == 32);
	REQUIRE(v.back() == 32);
}

template<size_t BlockSize>
class overallocate_test_resource : public test_resource
{
	bool m_did_overallocate = false;
	bool m_did_overdeallocate = false;
public:
	bool was_last_alloc_over() const noexcept { return m_did_overallocate; }
	bool was_last_dealloc_over() const noexcept { return m_did_overdeallocate; }


	[[nodiscard]] kab::byte_span allocate(size_t n, kab::align_t alignment)
	{
		m_did_overallocate = false;
		m_last_alloc = n;
		m_last_alloc_align = static_cast<size_t>(alignment);
		m_current_alloc += n;
		m_total_alloc += n;
		return test_resource::do_allocate(n, alignment);
	}

	[[nodiscard]] kab::byte_span over_allocate(size_t n, kab::align_t alignment)
	{
		m_did_overallocate = true;
		// Requested alloc
		m_last_alloc = n;
		m_last_alloc_align = static_cast<size_t>(alignment);
		// Actual alloc
		const size_t actual_alloc = std::max(n, BlockSize);
		m_current_alloc += actual_alloc;
		m_total_alloc += actual_alloc;
		return test_resource::do_allocate(actual_alloc, alignment);
	}

	void deallocate(kab::byte_span s, kab::align_t alignment)
	{
		m_did_overdeallocate = false;
		m_last_dealloc = s.size;
		m_last_dealloc_align = static_cast<size_t>(alignment);
		m_current_alloc -= s.size;
		return test_resource::do_deallocate(s, alignment);
	}

	void over_deallocate(kab::byte_span s, kab::align_t alignment)
	{
		if (s.size == 0)
		{
			return;
		}

		m_did_overdeallocate = true;
		m_last_dealloc = s.size;
		m_last_dealloc_align = static_cast<size_t>(alignment);
		m_current_alloc -= s.size;
		return test_resource::do_deallocate(s, alignment);
	}
};

static constexpr auto overallocate_size = 64;

template<typename T>
using over_vector = kab::vector<T, kab::resource_reference<overallocate_test_resource<overallocate_size>>>;

TEST_CASE("Container Vector Overallocate", "[container]")
{
	overallocate_test_resource<overallocate_size> r;

	over_vector<kab::byte> v(r);
	v.reserve(16); // over-allocate at least 16 bytes, which is under 'overallocate_size'
	const size_t current_alloc = r.get_total_alloc();
	REQUIRE(r.was_last_alloc_over());
	REQUIRE(r.get_last_alloc() == 16); // expected the requested alignment to be 16
	REQUIRE(v.capacity() == overallocate_size); // expected vector to use the extra capacity
}