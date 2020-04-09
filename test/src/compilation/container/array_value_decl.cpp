#include "array_value_decl.h"

kab::array_value<int, kab::new_resource> array_value_decl()
{
	int const value[] = { 1 };
	auto const v = kab::array_value<int, kab::new_resource>::from_range(value);
	kab::array_value<int, kab::new_resource> v2;
	v2.assign(value);
	return v;
}
