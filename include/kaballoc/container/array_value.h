#pragma once

#include "kaballoc/container/array_value.decl.h"
#include "kaballoc/container/detail/array_value.inl.h"

/**
 * Macro to define a specialization of the 'array_value' template
 *
 * By having this KAB_CONTAINER_ARRAY_VALUE_IMPL in a compiled object, other
 * translation units are free to use only the declaration header, not having to import the entire template
 *
 * The template signature of 'array_value' is not guaranteed, so use this macro rather than making your own declarations
 */
#define KAB_CONTAINER_ARRAY_VALUE_IMPL(ElementType, ResourceType) \
	namespace kab { \
		template class array_value<ElementType, ResourceType>; \
	}