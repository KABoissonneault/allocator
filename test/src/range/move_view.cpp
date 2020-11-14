#include "kaballoc/range/move_view.h"

#include <catch.hpp>

#include <vector>
#include <memory>
#include <algorithm>

TEST_CASE("move_view", "[range]")
{
	std::vector<std::unique_ptr<int>> v(10);
	std::generate(v.begin(), v.end(), [n = 0]() mutable -> std::unique_ptr<int> { return std::make_unique<int>(n++); });

	std::vector<std::unique_ptr<int>> v2;

	kab::move_view m(v);
	v2.insert(v2.end(), m.begin(), m.end());

	REQUIRE(v2.size() == 10);
	for (int n = 0; n < 10; ++n)
	{
		REQUIRE(v[n] == nullptr);
		REQUIRE(*v2[n] == n);
	}
}
