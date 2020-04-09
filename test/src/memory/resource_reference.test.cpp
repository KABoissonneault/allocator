#include "kaballoc/memory/resource_reference.h"

#include <catch.hpp>

struct simple_alloc
{
	kab::byte_span allocate(kab::size_t, kab::align_t);
	void deallocate(kab::byte_span, kab::align_t) noexcept;
};

TEST_CASE("Resource Reference Compilation", "[memory]")
{
	using resource1 = kab::resource_reference<simple_alloc>;

	REQUIRE(!std::is_default_constructible_v<resource1>);
	REQUIRE(std::is_nothrow_copy_constructible_v<resource1>);
	REQUIRE(std::is_nothrow_copy_assignable_v<resource1>);
	REQUIRE(std::is_nothrow_move_constructible_v<resource1>);
	REQUIRE(std::is_nothrow_move_assignable_v<resource1>);
}