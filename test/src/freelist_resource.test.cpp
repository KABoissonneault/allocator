#include <memory/resource_reference.h>
#include <memory/freelist_resource.h>

#include <catch.hpp>

#include "test_resource.h"

constexpr size_t BlockSize = 64;
using freelist_resource = kab::freelist_resource<kab::resource_reference<test_resource>, BlockSize>;

TEST_CASE("Freelist BlockSize Alloc", "[memory]")
{	
	test_resource tester;
	
	{
		freelist_resource freelist(tester);

		// Expect no allocations so far
		REQUIRE(tester.get_total_alloc() == 0);

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
	}

	REQUIRE(tester.get_current_alloc() == 0);

}

TEST_CASE("Freelist Multi Alloc", "[memory]")
{
	test_resource tester;

	{
		freelist_resource freelist(tester);

		constexpr size_t AllocCount = 10;
		kab::byte_span allocations[AllocCount];
		for (kab::byte_span& alloc : allocations)
		{
			alloc = freelist.allocate(BlockSize);
		}

		REQUIRE(tester.get_current_alloc() == AllocCount * BlockSize);
		for (kab::byte_span alloc : allocations)
		{
			REQUIRE(alloc.size == BlockSize);
		}

		for (kab::byte_span alloc : allocations)
		{
			freelist.deallocate(alloc);
		}

		REQUIRE(tester.get_current_alloc() == AllocCount * BlockSize);

		const kab::byte_span freelist_head = freelist.allocate(BlockSize);
		REQUIRE(freelist_head.data == allocations[AllocCount - 1].data); // head of the freelist should be the last deallocation
		freelist.deallocate(freelist_head);
	}

	REQUIRE(tester.get_current_alloc() == 0);
}

TEST_CASE("Freelist Small Alloc", "[memory]")
{
	test_resource tester;

	{
		freelist_resource freelist(tester);

		constexpr size_t SmallAlloc = BlockSize / 2;
		const kab::byte_span first_alloc = freelist.allocate(SmallAlloc);

		REQUIRE(first_alloc.size == SmallAlloc); // 'allocate' always returns the requested size
		REQUIRE(tester.get_last_alloc() == BlockSize); // freelist only allocates in chunks of "BlockSize"
		REQUIRE(tester.get_current_alloc() == BlockSize);

		freelist.deallocate(first_alloc);

		REQUIRE(tester.get_current_alloc() == BlockSize); // freelist never frees chunks that fit the block size

		const kab::byte_span second_alloc = freelist.allocate(SmallAlloc);

		REQUIRE(tester.get_current_alloc() == BlockSize); // expect no new allocations: we have to recover the last deallocated block
		REQUIRE(second_alloc.data == first_alloc.data); // expect to get back the same block

		freelist.deallocate(second_alloc);

		freelist.clear();
		REQUIRE(tester.get_current_alloc() == 0);

		freelist.deallocate(freelist.allocate(BlockSize));
	}

	REQUIRE(tester.get_current_alloc() == 0);
}