#include "kaballoc/memory/resource_reference.h"
#include "kaballoc/memory/freelist_resource.h"

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

		const kab::byte_span first_alloc = freelist.allocate(BlockSize, kab::default_align_v);

		REQUIRE(first_alloc.size == BlockSize); // 'allocate' always returns the requested size
		REQUIRE(tester.get_last_alloc() == BlockSize); // freelist only allocates in chunks of "BlockSize"
		REQUIRE(static_cast<kab::align_t>(tester.get_last_alloc_align()) >= kab::default_align_v); // alignment needs to be respected
		REQUIRE(tester.get_current_alloc() == BlockSize);

		freelist.deallocate(first_alloc, kab::default_align_v);

		REQUIRE(tester.get_current_alloc() == BlockSize); // freelist never frees chunks that fit the block size

		const kab::byte_span second_alloc = freelist.allocate(BlockSize, kab::default_align_v);

		REQUIRE(tester.get_current_alloc() == BlockSize); // expect no new allocations: we have to recover the last deallocated block
		REQUIRE(second_alloc.data == first_alloc.data); // expect to get back the same block
		
		freelist.deallocate(second_alloc, kab::default_align_v);

		freelist.clear();
		REQUIRE(tester.get_current_alloc() == 0);

		freelist.deallocate(freelist.allocate(BlockSize, kab::default_align_v), kab::default_align_v);
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
			alloc = freelist.allocate(BlockSize, kab::default_align_v);
		}

		REQUIRE(tester.get_current_alloc() == AllocCount * BlockSize);
		for (kab::byte_span alloc : allocations)
		{
			REQUIRE(alloc.size == BlockSize);
		}

		for (kab::byte_span alloc : allocations)
		{
			freelist.deallocate(alloc, kab::default_align_v);
		}

		REQUIRE(tester.get_current_alloc() == AllocCount * BlockSize);

		const kab::byte_span freelist_head = freelist.allocate(BlockSize, kab::default_align_v);
		REQUIRE(freelist_head.data == allocations[AllocCount - 1].data); // head of the freelist should be the last deallocation
		freelist.deallocate(freelist_head, kab::default_align_v);
	}

	REQUIRE(tester.get_current_alloc() == 0);
}

TEST_CASE("Freelist Small Alloc", "[memory]")
{
	test_resource tester;

	{
		freelist_resource freelist(tester);

		constexpr size_t SmallAlloc = BlockSize / 2;
		const kab::byte_span first_alloc = freelist.allocate(SmallAlloc, kab::default_align_v);

		REQUIRE(first_alloc.size == SmallAlloc); // 'allocate' always returns the requested size
		REQUIRE(tester.get_last_alloc() == BlockSize); // freelist only allocates in chunks of "BlockSize"
		REQUIRE(tester.get_current_alloc() == BlockSize);

		freelist.deallocate(first_alloc, kab::default_align_v);

		REQUIRE(tester.get_current_alloc() == BlockSize); // freelist never frees chunks that fit the block size

		const kab::byte_span second_alloc = freelist.allocate(SmallAlloc, kab::default_align_v);

		REQUIRE(tester.get_current_alloc() == BlockSize); // expect no new allocations: we have to recover the last deallocated block
		REQUIRE(second_alloc.data == first_alloc.data); // expect to get back the same block

		freelist.deallocate(second_alloc, kab::default_align_v);

		freelist.clear();
		REQUIRE(tester.get_current_alloc() == 0);

		freelist.deallocate(freelist.allocate(BlockSize, kab::default_align_v), kab::default_align_v);
	}

	REQUIRE(tester.get_current_alloc() == 0); // test that the destructor cleans the freelist
}

TEST_CASE("Freelist Big Alloc", "[memory]")
{
	test_resource tester;

	{
		freelist_resource freelist(tester);

		constexpr size_t BigAlloc = BlockSize * 2;
		const kab::byte_span first_alloc = freelist.allocate(BigAlloc, kab::default_align_v);

		REQUIRE(first_alloc.size == BigAlloc); // 'allocate' always returns the requested size
		REQUIRE(tester.get_last_alloc() == BigAlloc); // freelist has no choice but to rely on the inner allocator for bigger allocs

		freelist.deallocate(first_alloc, kab::default_align_v);

		REQUIRE(tester.get_current_alloc() == 0); // freelist has no choice but to deallocate this block

		freelist.deallocate(freelist.allocate(BlockSize, kab::default_align_v), kab::default_align_v);
	}

	REQUIRE(tester.get_current_alloc() == 0);
}
