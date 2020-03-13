#pragma once

#include <new>

class new_observer 
{
public:
	virtual void on_new(std::size_t size, std::align_val_t alignment) = 0;
	virtual void on_delete(void* ptr, std::size_t size, std::align_val_t alignment) = 0;
};

void set_new_observer(new_observer& obs);
void clear_new_observer();