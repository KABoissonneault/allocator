#pragma once

// Add std types as relocatable based on the table found on this site: https://quuxplusone.github.io/blog/2019/02/20/p1144-what-types-are-relocatable/

#include "kaballoc/trait/relocatable.h"

#include <variant>

namespace kab
{
	template<typename... Args>
	using variant = std::variant<Args...>;

	template<typename... Args>
	struct is_trivially_relocatable<std::variant<Args...>> : std::conjunction<is_trivially_relocatable<Args>...> {};
}