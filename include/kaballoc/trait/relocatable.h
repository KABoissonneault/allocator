#pragma once

#include <type_traits>

#include "kaballoc/core/compiler.h"

namespace kab
{
	/**
	 * is_trivially_relocatable
	 *
	 * Given,
	 *   - a type T
	 *   - a variable v1 and v2 of type T
	 *   - a function relocate(src, dest), where 'src' is a pointer to a valid object of type T and 'dest' is a pointer to arbitrary storage appropriate for T
	 *
	 *  Relocation is an operation where the lifetime of the object itself is transferred to another location. 
	 *  This is different from a move operation, which transfers sub-resources of an object to another object, but does not affect lifetime of the object itself.
	 *  After relocation, the source object must *not* be destroyed. Note: For this reason, relocation is not appropriate for automatic or static lifetime variables.
	 *  Pointers, references, iterators, and other similar reference-types to the source object are all invalidated 
	 *
	 *  The type T is trivially relocatable if 'relocate' can be implemented as memcpy(dest, src, sizeof(T)).
	 *  Even a non-trivially copyable type, like a string or a vector, can often be trivially relocated. 
	 */
	template<typename T>
	struct is_trivially_relocatable : std::false_type {};

	// Fundamental specializations
	template<> struct is_trivially_relocatable<decltype(nullptr)> : std::true_type {};
	template<> struct is_trivially_relocatable<char> : std::true_type {};
	template<> struct is_trivially_relocatable<unsigned char> : std::true_type {};
	template<> struct is_trivially_relocatable<signed char> : std::true_type {};
#if !KAB_COMPILER_MSVC || defined(_NATIVE_WCHAR_T_DEFINED) // on MSVC, wchar_t might not be a fundamental type
	template<> struct is_trivially_relocatable<wchar_t> : std::true_type {};
#endif
	template<> struct is_trivially_relocatable<char16_t> : std::true_type {};
	template<> struct is_trivially_relocatable<char32_t> : std::true_type {};
	// template<> struct is_trivially_relocatable<char8_t> : std::true_type {}; // c++20
	template<> struct is_trivially_relocatable<signed short> : std::true_type {};
	template<> struct is_trivially_relocatable<unsigned short> : std::true_type {};
	template<> struct is_trivially_relocatable<signed int> : std::true_type {};
	template<> struct is_trivially_relocatable<unsigned int> : std::true_type {};
	template<> struct is_trivially_relocatable<signed long> : std::true_type {};
	template<> struct is_trivially_relocatable<unsigned long> : std::true_type {};
	template<> struct is_trivially_relocatable<signed long long> : std::true_type {};
	template<> struct is_trivially_relocatable<unsigned long long> : std::true_type {};
	template<> struct is_trivially_relocatable<float> : std::true_type {};
	template<> struct is_trivially_relocatable<double> : std::true_type {};
	template<> struct is_trivially_relocatable<long double> : std::true_type {};
	template<> struct is_trivially_relocatable<bool> : std::true_type {};

	template<typename T>
	inline constexpr bool is_trivially_relocatable_v = is_trivially_relocatable<T>::value;
}
