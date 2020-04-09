#include "vector_decl.h"

#include <iostream>

kab::vector<int, kab::new_resource> vector_decl()
{
	kab::vector<int, kab::new_resource> v;
	v.push_back(1);
	auto const v2 = kab::vector<int, kab::new_resource>::from_container(v);
	v.assign(v2);
	std::cout << v.back();
	std::cout << v.front();
	std::cout << v.capacity();
	v.emplace_back(0);

	return v;
}
