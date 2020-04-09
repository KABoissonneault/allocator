#include "container/vector.h"

#include "memory/new_resource.h"

namespace kab
{
	template class vector<int, new_resource>;
}
