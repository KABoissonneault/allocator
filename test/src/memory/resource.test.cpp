#include "kaballoc/memory/resource.h"

#include <catch.hpp>
#include <cstddef>

TEST_CASE("Default alignment", "[memory]")
{
	REQUIRE(static_cast<size_t>(kab::default_align_v) == alignof(std::max_align_t));
}