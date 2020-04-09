#pragma once

#include "kaballoc/container/array_value.decl.h"
#include "kaballoc/memory/new_resource.h"

namespace kab
{
	extern template class array_value<int, new_resource>;
}

kab::array_value<int, kab::new_resource> array_value_decl();