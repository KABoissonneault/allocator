#include "kaballoc/std/unique_ptr.h"

static_assert(kab::is_trivially_relocatable_v<kab::unique_ptr<int>>);