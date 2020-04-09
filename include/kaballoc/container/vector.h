#pragma once

#include "kaballoc/container/vector.decl.h"
#include "kaballoc/container/detail/vector.inl.h"

/**
 * Macro to define a specialization of the 'vector' template
 *
 * By having this KAB_CONTAINER_VECTOR_IMPL in a compiled object, other
 * translation units are free to use only the declaration header, not having to import the entire template
 *
 * The template signature of 'vector' is not guaranteed, so use this macro rather than making your own declarations
 */
#define KAB_CONTAINER_VECTOR_IMPL(ElementType, ResourceType) \
	namespace kab { \
		template class vector<ElementType, ResourceType>; \
	}