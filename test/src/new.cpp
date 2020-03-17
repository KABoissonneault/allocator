#include "new.h"

#include <cstdlib>

static new_observer* s_observer; 

void* operator new(std::size_t n, std::align_val_t align)
{
	if (s_observer != nullptr)
	{
		s_observer->on_new(n, align);
	}

#if defined(_MSC_VER)
	return _aligned_malloc(n, static_cast<std::size_t>(align));
#elif
	return std::aligned_alloc(static_cast<std::size_t>(align), n);
#endif
}

void operator delete(void* ptr, std::size_t n, std::align_val_t align)
{
	if (s_observer != nullptr)
	{
		s_observer->on_delete(ptr, n, align);
	}

#if defined(_MSC_VER)
	_aligned_free(ptr);
#else
	std::free(ptr);
#endif
}

void set_new_observer(new_observer& obs)
{
	s_observer = &obs;
}

void clear_new_observer()
{
	s_observer = nullptr;
}