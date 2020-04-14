#pragma once

#include "kaballoc/core/size_t.h"

namespace kab
{
	template<typename T>
	void destroy_at(T* p)
	{
		p->~T();
	}

	template<typename T>
	void destroy(T* it, T* sent)
	{
		for (; it != sent; ++it) {
			kab::destroy_at(it);
		}
	}

	template<typename T>
	void destroy_n(T* it, size_t n) 
	{
		for (; n > 0; --n, ++it) {
			kab::destroy_at(it);
		}
	}
}