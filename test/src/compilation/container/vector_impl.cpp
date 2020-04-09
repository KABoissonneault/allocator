#include "kaballoc/container/vector.h"
#include "kaballoc/memory/new_resource.h"

namespace kab
{
	template class vector<int, new_resource>;
}
