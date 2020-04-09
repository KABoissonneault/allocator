#include "vector_decl.h"

kab::vector<int, kab::new_resource> vector_decl()
{
	kab::vector<int, kab::new_resource> v;
	v.push_back(1);
	return v;
}
