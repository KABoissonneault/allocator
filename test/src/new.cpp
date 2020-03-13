#include "new.h"

#include <cstdlib>

static new_observer* s_observer; 

static void* operator new(std::size_t n, std::align_val_t align)
{
	if (s_observer != nullptr)
	{
		s_observer->on_new(n, align);
	}

	return std::aligned_alloc(static_cast<std::size_t>(align), n);
}

void set_new_observer(new_observer& obs)
{
	s_observer = &obs;
}

void clear_new_observer()
{
	s_observer = nullptr;
}