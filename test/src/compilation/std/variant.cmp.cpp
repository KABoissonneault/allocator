#include "kaballoc/std/variant.h"

static_assert(kab::is_trivially_relocatable_v<kab::variant<int, float, bool>>);
