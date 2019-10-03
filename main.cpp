
#include <stdio.h>
#include <PhysicalQuantity.h>
#include <PhysicalQuantity/hash.h>
typedef PhysicalQuantity PQ;

int main(int argc, char** argv)
{
	if (!PQHeaderOptionsMatch) 
	{
		printf("Error: library code was compiled with different header options.\n");
		return 1;
	}
	for (int iArg = 1; iArg < argc; iArg++)
	{
		if (!strcmp(argv[iArg], "test"))
		{
			//RunTestSuite();
		}
#if !defined(NO_HASHING) && !defined(NO_PRINTF)
		else if (!strcmp(argv[iArg], "hash"))
		{
			PhysicalQuantity_dumpHashTable(&PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].symbol, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitSymbols, "UnitSymbols");
			PhysicalQuantity_dumpHashTable(&PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].longName, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitLongNames, "UnitLongNames");
			PhysicalQuantity_dumpHashTable(&PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].symbol, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixSymbols, "PrefixSymbols");
			PhysicalQuantity_dumpHashTable(&PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].longName, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixLongNames, "PrefixLongNames");
			return 0;
		}
#endif //#ifndef NO_HASHING
		else
		{
			printf("Unknown option: %s", argv[iArg]);
			return 1;
		}
	}
	return 0;
}

/*
int xmain()
{
	PhysicalQuantity::cstrHasherTiny h;
	auto r = h("gram");
	r = h("joule");
	r = h("ampere");
	return 0;
}
void mang()
{
	double x = csubstr("-1.24e-3.2").atof();
	printf("2.5e11 mang --> %s\n", PQ("2.5e11 mang").toString().c_str());
}


void asdlkfhasd()
{
	PhysicalQuantity q("12 N m");
	auto m = 2.0_m;
	auto n = 6_N;
	auto p = n * m;
	if (p == q) { printf("Success!\n"); }
	else { printf("Fail.\n"); }
	printf("%s\n", p.toString().c_str());
	PhysicalQuantity par;
	par.parse(p.toString().c_str());
	printf("Parsed: %s\n", par.toString().c_str());
	
	// TODO: Test deka- da-
	return 0;
}

int main02()
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

int main03()
{
	printf("%u\n", (unsigned int)PhysicalQuantity::cstrHasherTiny{}("km"));
	printf("%u\n", (unsigned int)PhysicalQuantity::cstrHasherTiny{}("N"));
	printf("%u\n", (unsigned int)PhysicalQuantity::cstrHasherTiny{}("Js"));
	printf("%u\n", (unsigned int)PhysicalQuantity::cstrHasherTiny{}("degF"));
	printf("%u\n", (unsigned int)PhysicalQuantity::cstrHasherTiny{}("mph"));
	printf("%u\n", (unsigned int)PhysicalQuantity::cstrHasherTiny{}("kph"));
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
*/
