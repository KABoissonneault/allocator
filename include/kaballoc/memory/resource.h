#pragma once

#include "kaballoc/memory/memory_common.h"
#include "kaballoc/memory/byte_span.h"

namespace kab
{
	// Strong typedef for an alignment value
	enum class align_t : size_t {};

	template<typename T>
	inline constexpr align_t align_v{ alignof(T) };

	inline constexpr bool operator==(align_t lhs, align_t rhs) noexcept
	{
		return static_cast<size_t>(lhs) == static_cast<size_t>(rhs);
	}

	inline constexpr bool operator!=(align_t lhs, align_t rhs) noexcept
	{
		return static_cast<size_t>(lhs) != static_cast<size_t>(rhs);
	}

	inline constexpr bool operator<(align_t lhs, align_t rhs) noexcept
	{
		return static_cast<size_t>(lhs) < static_cast<size_t>(rhs);
	}

	inline constexpr bool operator<=(align_t lhs, align_t rhs) noexcept
	{
		return static_cast<size_t>(lhs) <= static_cast<size_t>(rhs);
	}

	inline constexpr bool operator>(align_t lhs, align_t rhs) noexcept
	{
		return static_cast<size_t>(lhs) > static_cast<size_t>(rhs);
	}

	inline constexpr bool operator>=(align_t lhs, align_t rhs) noexcept
	{
		return static_cast<size_t>(lhs) >= static_cast<size_t>(rhs);
	}

	// The biggest natural alignment. Any alignment value above this is considered "over-alignment"
	inline constexpr align_t max_align_v{ alignof(long double) };

	inline constexpr bool is_power_of_two(size_t n) noexcept
	{
		return n != 0 && ((n & (n - 1)) == 0);
	}

	/** concept memory_resource
	 * 
	 *  A memory_resource type is a type which has access to a region of heap and can allocate (and deallocate) sections of it on demand
	 *
	 *  The essential functions are:
	 *      byte_span allocate(size_t s, align_t a)
	 *          - Which is an allocation function
	 *          - Where 's' is the requested size of the allocation
	 *          - Where 'a' is the requested alignment of the allocation
	 *          - The returned byte_span has a size (ie: returned size) equal to the requested size
	 *          - The returned byte_span has a pointer to storage (ie: returned storage) with alignment that respects the requested alignment
	 *      void deallocate(byte_span s, align_t a)
	 *          - Which is a deallocation function
	 *          - Where 's' is a span with the values returned by a call to an allocation function
	 *          - Where 'a' was the alignment requested on the allocation function
	 *			- If the size of 's' is 0, the deallocation must have no effects. In that case, its pointer may have any value
	 *  
	 *  Memory resources are meant to be used as "value types", that is, they're always passed by value. If reference semantics are desired, see resource_reference.
	 *  Two memory resources are considered equivalent if memory allocated from one resource can be deallocated from the other. Empty (aka monostate) resources are all equivalent.
	 *
	 *  Memory resources can have extensions.
	 *  
	 *  Relocatable
	 *
	 *  If the memory resource is relocatable, it must be trivially relocatable. The relocated resource must be equivalent to the original.
	 *
	 *  Moveable
	 * 
	 *  A memory resource is moveable if it is relocatable, noexcept move constructible, and noexcept move assignable. The moved resource must be equivalent to the original.
	 *
	 *  Copyable
	 *
	 *  A memory resource is copyable if it is moveable, noexcept copy constructible, and noexcept copy assignable. The copied resource must be equivalent to the original.
	 *
	 *  Over Allocator
	 *  
	 *  An over-allocator memory resource is a memory resource that supports the following functions:
	 *      byte_span over_allocate(size_t s, align_t a)
	 *          - Which is an allocation function. It behaves like 'allocate', except the returned size can be bigger than the requested size
	 *          - On deallocation, the span provided to the deallocation function must still be the returned one, not the requested one
	 *      void over_deallocate(byte_span s, align_t a)
	 *          - Which is an optional deallocation function. If provided, the user must call this instead of 'deallocate' when using over-allocations
	 *          - Otherwise, behaves like 'deallocate'
	 *
	 *  Comparison
	 *
	 *  An allocator may support operator== to compare two resources of the same type for equivalence. 
	 *  Resources of an empty (as in std::is_empty) memory resource type are always equivalent, no operator== needed.
	 *  Resources of non-empty type with no operator== are always considered non-equivalent.
	 */
}
