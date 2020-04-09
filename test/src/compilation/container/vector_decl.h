#pragma once

#include "kaballoc/container/vector.decl.h"
#include "kaballoc/memory/new_resource.h"

namespace kab
{
	extern template class vector<int, new_resource>;
}

kab::vector<int, kab::new_resource> vector_decl();
