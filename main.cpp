
#include <stdio.h>
#include "include/PhysicalQuantity.h"

int main01()
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

int main()
{
	const char* teststr = "Hello world.";
	CSubString s(teststr, 1, 2);
	char buf[100];
	s.copy(buf, 100);
	printf("%s\n", buf);
	if (s.begins("elsdrtf")) { printf("Begins\n"); }
	if (s.ends("sdfgsdgel")) { printf("Ends\n"); }
	if (s.begins("dvbdfb")) {printf("Begin fail\n"); }
	if (s.ends("dfgbdfbhg")) { printf("End fail\n"); }
	if (s.begins("a")) {printf("Begin fail\n"); }
	if (s.ends("a")) { printf("End fail\n"); }
	if (s.begins("")) {printf("Begin fail\n"); }
	if (s.ends("")) { printf("End fail\n"); }
	if (s == "el") {printf("Equal pass\n"); }
	if (s.begins("el")) { printf("Begins equal\n"); }
	if (s.ends("el")) { printf("ends equal\n"); }
	printf(" %d ", s.at("el"));
	printf(" %d ", s.at("dfgvbdfel"));
	printf(" %d ", s.at("eldfgdfg"));
	printf(" %d ", s.at("dfgdfelfdgdfg"));
	printf(" %d ", s.at("sdfkvhfuivh"));
	printf(" %d ", s.at(""));
	printf(" %d ", s.at("e"));

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
