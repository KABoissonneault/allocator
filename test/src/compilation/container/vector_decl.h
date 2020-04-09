#pragma once

#include "kaballoc/container/vector.decl.h"
#include "kaballoc/memory/new_resource.h"

KAB_CONTAINER_VECTOR_DECL(int, kab::new_resource)

kab::vector<int, kab::new_resource> vector_decl();
