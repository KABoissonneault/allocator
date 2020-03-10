#include <memory/freelist_resource.h>
#include <memory/malloc_resource.h>

#include <catch.hpp>

#include "test_resource.h"

TEST_CASE("Freelist resource", "[memory]")
{
	using resource = kab::freelist_resource<test_resource, 64>;
}
