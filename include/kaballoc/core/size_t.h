#pragma once

namespace kab
{
	using size_t = decltype(sizeof(void*));
	inline constexpr size_t size_t_max_v = static_cast<size_t>(-1);

	// assume 64 bits for now
#	define KAB_ATOMIC_FETCH_INC_SIZE_T_RELAXED(v) KAB_ATOMIC_FETCH_INC_UINT64_RELAXED(v)
#	define KAB_ATOMIC_FETCH_DEC_SIZE_T_RELEASE(v) KAB_ATOMIC_FETCH_DEC_UINT64_RELEASE(v)
}
