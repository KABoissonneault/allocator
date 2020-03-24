#pragma once

#include <type_traits>

namespace kab
{
	/**
	 * is_trivially_relocatable
	 *
	 * Given,
	 *   - a type T
	 *   - a variable v1 and v2 of type T
	 *   - a function relocate(src, dest) which takes a T lvalue and a pointer to arbitrary storage and returns void
	 *
	 *  Relocation is an operation where the lifetime of the object itself is transferred to another location. 
	 *  This is different from a move operation, which transfers sub-resources of an object to another object, but does not affect lifetime of the object itself.
	 *  After relocation, the source object must *not* be destroyed. Note: For this reason, relocation is not appropriate for automatic or static lifetime variables.
	 *  Pointers, references, iterators, and other similar reference-types are all invalidated 
	 *
	 *  The type T is trivially relocatable if 'relocate' can be implemented as memcpy(dest, src).
	 *  Even a non-trivially copyable type, like a string or a vector, can often be trivially relocated. 
	 */
	template<typename T>
	struct is_trivially_relocatable : std::false_type
	{

	};

	// Fundamental specializations
	template<> struct is_trivially_relocatable<char> : std::true_type {};
	template<> struct is_trivially_relocatable<unsigned char> : std::true_type {};
	template<> struct is_trivially_relocatable<signed char> : std::true_type {};
	template<> struct is_trivially_relocatable<signed short> : std::true_type {};
	template<> struct is_trivially_relocatable<unsigned short> : std::true_type {};
	template<> struct is_trivially_relocatable<signed int> : std::true_type {};
	template<> struct is_trivially_relocatable<unsigned int> : std::true_type {};
	template<> struct is_trivially_relocatable<signed long> : std::true_type {};
	template<> struct is_trivially_relocatable<unsigned long> : std::true_type {};
	template<> struct is_trivially_relocatable<signed long long> : std::true_type {};
	template<> struct is_trivially_relocatable<unsigned long long> : std::true_type {};
}