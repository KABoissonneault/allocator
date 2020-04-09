#include "kaballoc/container/array_value.h"
#include "kaballoc/memory/new_resource.h"

namespace kab
{
	template class array_value<int, kab::new_resource>;
}