
#include <stdio.h>
#include "include/PhysicalQuantity.h"

int main()
{
	PhysicalQuantity q("12 N m");
	auto m = 2.0_m;
	auto n = 6_N;
	auto p = n * m;
	if (p == q) { printf("Success!\n"); }
	else { printf("Fail.\n"); }
	printf("%s\n", p.toString().c_str());
	PhysicalQuantity par;
	par.parse(p.toString());
	printf("Parsed: %s\n", par.toString().c_str());

	return 0;
}


void test1()
{
	PhysicalQuantity::cstrHasherTiny th;
	size_t h;
	h = th("m");
	h = th("km");
	h = th("N");
	h = th("MN");
	h = th("J");
	h = th("s");
	h = th("deg");
	return;
}
