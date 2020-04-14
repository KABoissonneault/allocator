#include "vector_decl.h"

volatile int vector_decl_observe;

kab::vector<int, kab::new_resource> vector_decl()
{
	kab::vector<int, kab::new_resource> v;
	v.push_back(1);
	auto const v2 = kab::vector<int, kab::new_resource>::from_container(v);
	v.assign(v2);
	vector_decl_observe = v.back();
	vector_decl_observe = v.front();
	vector_decl_observe = static_cast<int>(v.capacity());
	v.emplace_back(0);

	return v;
}
