#include <memory/resource_reference.h>
#include <memory/freelist_resource.h>
#include <memory/malloc_resource.h>

#include <catch.hpp>

#include "test_resource.h"

TEST_CASE("Freelist resource", "[memory]")
{
	constexpr size_t BlockSize = 64;
	using freelist_resource = kab::freelist_resource<kab::resource_reference<test_resource>, BlockSize>;

	test_resource tester;
	freelist_resource freelist(tester);

	// Expect no allocations so far
	REQUIRE(tester.get_total_alloc() == 0);

	// Test "BlockSize" sized allocations
	{
		const kab::byte_span first_alloc = freelist.allocate(BlockSize);

		REQUIRE(first_alloc.size == BlockSize); // 'allocate' always returns the requested size
		REQUIRE(tester.get_last_alloc() == BlockSize); // freelist only allocates in chunks of "BlockSize"
		REQUIRE(tester.get_current_alloc() == BlockSize);

		freelist.deallocate(first_alloc);

		REQUIRE(tester.get_current_alloc() == BlockSize); // freelist never frees chunks that fit the block size

		const kab::byte_span second_alloc = freelist.allocate(BlockSize);

		REQUIRE(tester.get_current_alloc() == BlockSize); // expect no new allocations: we have to recover the last deallocated block
		REQUIRE(second_alloc.data == first_alloc.data); // expect to get back the same block
		
		freelist.deallocate(second_alloc);

		freelist.clear();
		REQUIRE(tester.get_current_alloc() == 0);

		freelist.deallocate(freelist.allocate(BlockSize));
		freelist.~freelist_resource();
		REQUIRE(tester.get_current_alloc() == 0);

		new(&freelist) freelist_resource(tester);
	}
}
