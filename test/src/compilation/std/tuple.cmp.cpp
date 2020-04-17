#include "kaballoc/std/tuple.h"

static_assert(kab::is_trivially_relocatable_v<kab::tuple<int, float, bool>>);
