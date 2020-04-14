#include "kaballoc/core/comparison.h"

#include <catch.hpp>

#include <variant>

namespace
{
	struct U; 

	struct T 
	{
		int value;
		bool is_u = false;
	};

	bool operator==(T lhs, T rhs) noexcept
	{
		return lhs.value == rhs.value && lhs.is_u && rhs.is_u;
	}

	bool operator!=(T lhs, T rhs) noexcept
	{
		return !(lhs == rhs);
	}

	struct U 
	{
		int value;

		operator T() const { return T{ value, true }; }
	};

	bool operator<(T lhs, U rhs) noexcept 
	{
		return lhs.value < rhs.value;
	}

	bool operator<(U lhs, T rhs) noexcept 
	{
		return lhs.value < rhs.value;
	}
}

TEST_CASE("Min", "[core]")
{
	{
		auto const result = kab::min(T{ 0 }, U{ 1 });
		REQUIRE(!result.is_u);
		REQUIRE(result.value == 0);
	}

	{
		auto const result = kab::min(T{ 100000 }, U{ 3 });
		REQUIRE(result.is_u);
		REQUIRE(result.value == 3);
	}
}

TEST_CASE("Max", "[core]")
{
	{
		auto const result = kab::max(T{ 1 }, U{ -34 });
		REQUIRE(!result.is_u);
		REQUIRE(result.value == 1);
	}

	{
		auto const result = kab::max(T{ 1000 }, U{ 1030690103 });
		REQUIRE(result.is_u);
		REQUIRE(result.value == 1030690103);
	}
}

TEST_CASE("Minmax", "[core]")
{
	REQUIRE(kab::min(T{ 0 }, U{ 0 }) != kab::max(T{ 0 }, U{ 0 }));
}
