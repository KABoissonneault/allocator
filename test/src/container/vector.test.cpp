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

	// Move of default value
	vector<int> test_vec2(std::move(test_vec1));
	REQUIRE(test_vec2.get_resource() == kab::make_reference(resource));
	REQUIRE(test_vec2.size() == 0);
	REQUIRE(test_vec2.capacity() == 0);
	REQUIRE(resource.get_last_alloc() == 0);

	// Move of default value
	test_vec1 = std::move(test_vec2);
	REQUIRE(test_vec1.get_resource() == kab::make_reference(resource));
	REQUIRE(test_vec1.size() == 0);
	REQUIRE(test_vec1.capacity() == 0);
	REQUIRE(resource.get_last_alloc() == 0);
	
	// Self-assignment of default value
	test_vec1 = std::move(test_vec1);
	REQUIRE(test_vec1.get_resource() == kab::make_reference(resource));
	REQUIRE(test_vec1.size() == 0);
	REQUIRE(test_vec1.capacity() == 0);
	REQUIRE(resource.get_last_alloc() == 0);
}
