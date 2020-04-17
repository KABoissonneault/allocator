#pragma once

// Add std types as relocatable based on the table found on this site: https://quuxplusone.github.io/blog/2019/02/20/p1144-what-types-are-relocatable/

#include "kaballoc/trait/relocatable.h"

#include <memory>

namespace kab
{
	template<typename T>
	using default_delete = std::default_delete<T>;

	template<typename T>
	struct is_trivially_relocatable<default_delete<T>> : std::true_type {};

	template<typename T, typename D=default_delete<T>>
	using unique_ptr = std::unique_ptr<T, D>;

	template<typename T, typename D>
	struct is_trivially_relocatable<unique_ptr<T, D>> : std::bool_constant<is_trivially_relocatable_v<D>> {};
}