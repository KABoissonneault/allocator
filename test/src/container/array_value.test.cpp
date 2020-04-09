#include "kaballoc/container/array_value.h"

#include "test_resource.h"

#include <catch.hpp>

#include "kaballoc/memory/resource_reference.h"
#include "kaballoc/memory/new_resource.h"

template<typename T>
using array_value = kab::array_value<T, kab::resource_reference<test_resource>>;

TEST_CASE("Container Array Value Compilation", "[container]")
{
	REQUIRE(!std::is_default_constructible_v<array_value<int>>); // kab::resource_reference is not default constructible
	REQUIRE(std::is_default_constructible_v<kab::array_value<int, kab::new_resource>>);
	REQUIRE(std::is_nothrow_copy_constructible_v<array_value<int>>);
	REQUIRE(std::is_nothrow_copy_assignable_v<array_value<int>>);
	REQUIRE(std::is_nothrow_move_constructible_v<array_value<int>>);
	REQUIRE(std::is_nothrow_move_assignable_v<array_value<int>>);
	REQUIRE(std::is_nothrow_constructible_v<array_value<int>, test_resource&>);
	REQUIRE(std::is_nothrow_swappable_v<array_value<int>>);
}

TEST_CASE("Container Array Value Empty", "[container]")
{
	test_resource r;

	auto const test_empty = [](array_value<int const> const& v)
	{		
		REQUIRE(v.size() == 0);
		REQUIRE(v.is_empty());
		REQUIRE(v.begin() == v.end());		
	};

	auto const test_alloc = [&r](array_value<int const> const& v)
	{
		REQUIRE(v.get_resource() == kab::make_reference(r));
		REQUIRE(r.get_total_alloc() == 0);
	};

	array_value<int const> v(r);
	test_empty(v);
	test_alloc(v);

	array_value<int const> copy(v);
	test_empty(copy);
	test_alloc(copy);

	array_value<int const> move(std::move(copy));
	test_empty(move);
	test_alloc(move);

	copy = move;
	test_empty(copy);
	test_alloc(copy);

	v = v;
	test_empty(v);
	test_alloc(v);

	move = std::move(copy);
	test_empty(move);
	test_alloc(move);

	v = std::move(v);
	test_empty(v);
	test_alloc(v);

	v.swap(move);
	test_empty(v);
	test_alloc(v);	
	test_empty(move);
	test_alloc(move);
}
