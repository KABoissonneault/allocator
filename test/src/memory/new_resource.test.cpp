#include "memory/new_resource.h"

#include <catch.hpp>

TEST_CASE("New resource", "[memory]")
{
	kab::new_resource resource;
	kab::byte_span const s = resource.allocate(16, kab::max_align_v);
	resource.deallocate(s, kab::max_align_v);
}