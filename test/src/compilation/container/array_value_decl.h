#pragma once

#include "kaballoc/container/array_value.decl.h"
#include "kaballoc/memory/new_resource.h"

KAB_CONTAINER_ARRAY_VALUE_DECL(int, kab::new_resource)

kab::array_value<int, kab::new_resource> array_value_decl();