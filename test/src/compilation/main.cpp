#include "container/vector_decl.h"

#include <iostream>

int main()
{
	auto const v = vector_decl();
	std::cout << v[0];
}
