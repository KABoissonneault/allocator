#include "kaballoc/std/shared_ptr.h"

static_assert(kab::is_trivially_relocatable_v<kab::shared_ptr<int>>);
