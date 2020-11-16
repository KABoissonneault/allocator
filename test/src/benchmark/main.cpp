#include "kaballoc/memory/malloc_resource.h"

#include <cstdio>

namespace
{
	template<kab::align_t Alignment>
	void test_sequential(int limit)
	{
		std::printf("Sequential test. Alignment: %llu\n", static_cast<kab::size_t>(Alignment));

		kab::malloc_resource r;

		kab::byte_span s = r.over_allocate(0, Alignment);
		if (s.data != nullptr)
		{
			std::printf("Alloc 0: Supported. Actual size: %llu\n", s.size);
		}
		else
		{
			std::printf("Alloc 0: Unsupported\n");
		}

		for (int i = 1; i <= limit; ++i)
		{
			s = r.over_allocate(i, Alignment);

			std::printf("Alloc %d. Actual size: %llu\n", i, s.size);
		}
	}
}

int main()
{
	test_sequential<kab::default_align_v>(1024);
}
