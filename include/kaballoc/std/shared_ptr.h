#pragma once

// Add std types as relocatable based on the table found on this site: https://quuxplusone.github.io/blog/2019/02/20/p1144-what-types-are-relocatable/

#include "kaballoc/trait/relocatable.h"

#include <memory>

namespace kab
{
	template<typename T>
	using shared_ptr = std::shared_ptr<T>;

	template<typename T>
	using weak_ptr = std::weak_ptr<T>;

	template<typename T>
	struct is_trivially_relocatable<shared_ptr<T>> : std::true_type { };

	template<typename T>
	struct is_trivially_relocatable<weak_ptr<T>> : std::true_type { };
}