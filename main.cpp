
#include "include/PhysicalQuantity.h"

int main()
{
	PhysicalQuantity q("12 N m");
	auto m = 2.0_m;
	auto n = 6_N;
	auto p = n * m;

	PhysicalQuantity::cstrHasherTiny th;
	size_t h;
	h = th("m");
	h = th("km");
	h = th("N");
	h = th("MN");
	h = th("J");
	h = th("s");
	h = th("deg");
	return 0;
}
