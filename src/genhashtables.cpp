
// Note: Make sure PQ_GENERATING_HASH_TABLES is defined in project settings / 
// preprocessor options, or as a compiler argument,
// or you will get errors about undeclared identifiers *_HashTable
// It's not enough to declare it here. Some source .cpp files also need it.

#include <PhysicalQuantity.h>
#include <stdio.h>
#include <string.h>
#include <string>
//#include <PhysicalQuantity/hash.h>

#ifndef NO_HASHING

typedef PhysicalQuantity::CSubString csubstr;

#if defined(_MSC_VER) && defined(BEEP_IF_HASH_TABLES_REBUILT)
#include <Windows.h>
#endif


unsigned int dumpHashTable(const void* pArray, const char * const* pcstrMember, int elementSize, int arrayLen, int hashTableSize, const char* arrayName, size_t seed_p = 0, bool print = true)
{
	size_t l_seed;
	if (seed_p != 0) { l_seed = seed_p; }
	else { l_seed = PhysicalQuantity::defaultHashSeed; }

	int memberOfs = (int)((const char*)(pcstrMember) - (const char*)(pArray));
	int* hashes = new int[arrayLen];
	int* counts = new int[hashTableSize];
	for (int i = 0; i < arrayLen; i++)
	{
		hashes[i] = PhysicalQuantity::cstrHasherTiny(l_seed)(
			(*(const char**)((const char*)(pArray) + (elementSize * i) + memberOfs))
			) % (hashTableSize);
	}
	int maxCount = 0;
	int thiscount;
	for (int icount = 0; icount < hashTableSize; icount++)
	{
		thiscount = 0;
		for (int iu = 0; iu < arrayLen; iu++)
		{
			if (hashes[iu] == icount) { thiscount++;}
		}
		counts[icount] = thiscount;
		if (thiscount > maxCount) maxCount = thiscount;
	}
	if (print) printf("struct %s_HashTableEntry_t { unsigned int bucketSize; unsigned int bucket[%d]; };\n%s_HashTableEntry_t %s_HashTable[] = \n{", 
		arrayName, maxCount, arrayName, arrayName);
	int ib;
	for (int iTable = 0; iTable < hashTableSize; iTable++)
	{
		ib = 0;
		if (iTable != 0) { if (print) printf(","); }
		if (print) printf("\n{%u,{", counts[iTable]);
		for (int iu = 0; iu < arrayLen; iu++)
		{
			if ((hashes[iu]) == iTable)
			{
				if (ib != 0) { if (print) printf(","); }
				if (print) printf("%u", iu);
				ib++;
			}
		}
		while (ib < maxCount)
		{
			if (ib != 0) { if (print) printf(","); }
			if (print) printf("0");
			ib++;
		}
		if (print) printf("}}");
	}
	if (print) printf("\n};\n");
	delete[] hashes;
	delete[] counts;
	return maxCount;
}

unsigned int findSeed()
{
	unsigned int minSize = 0xFFFF;
	unsigned int minSeed = 0;
	for (int seed = 0; seed < 0xFFFF; seed++)
	{
		unsigned int r = dumpHashTable(&PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].symbol, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitSymbols, "UnitSymbols", seed, false);
		unsigned int r2 = dumpHashTable(&PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].longName, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitLongNames, "UnitLongNames", seed,  false);
		if (r2 > r) { r = r2; }
		r2 = dumpHashTable(&PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].symbol, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixSymbols, "PrefixSymbols", seed, false);
		if (r2 > r) { r = r2; }
		r2 = dumpHashTable(&PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].longName, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixLongNames, "PrefixLongNames", seed, false);
		if (r2 > r) { r = r2; }
		if (r < minSize)
		{
			minSize = r;
			minSeed = seed;
		}
	}
	return minSeed;
}


int main(int argc, char** argv)
{
	bool beep = false;
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--beep")) { beep = true; }
		else if (!strcmp(argv[i], "--find-seed"))
		{
			printf("0x%x", findSeed());
			return 0;
		}
		else { fprintf(stderr, "Unknown: %s\n", argv[i]); }
	}

	int ret = 0;
	try
	{
		dumpHashTable(&PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].symbol, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitSymbols, "UnitSymbols");
		dumpHashTable(&PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].longName, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitLongNames, "UnitLongNames");
		dumpHashTable(&PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].symbol, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixSymbols, "PrefixSymbols");
		dumpHashTable(&PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].longName, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixLongNames, "PrefixLongNames");

#if defined(_MSC_VER) && defined(BEEP_IF_HASH_TABLES_REBUILT)
		if (beep)
		{
			Beep(440, 150),
			Beep((int)(440 * (pow(2,(5.0/12)))), 150);
		}
#endif
	}
	catch (const std::exception& e)
	{
		fprintf(stderr, "%s", e.what());
		ret = 1;
#if defined(_MSC_VER) && defined(BEEP_IF_HASH_TABLES_REBUILT)
		if (beep)
		{
			Beep(350, 100);
			Beep(300, 100);
			Beep(350, 100);
			Beep(300, 100);
			Beep(350, 100);
			Beep(300, 100);
		}
#endif

	}
	return ret;
}

#else //#ifndef NO_HASHING
int main(int argc, char** argv)
{
	bool beep = false;
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--beep")) { beep = true; }
		else { fprintf(stderr, "Unknown: %s\n", argv[i]); }
	}

	fprintf(stderr, "Warning: Compiled without hashing, I can do nothing!\n");

#if defined(_MSC_VER) && defined(BEEP_IF_HASH_TABLES_REBUILT)
	if (beep)
	{
		Beep(350, 100);
		Beep(300, 200);
		Beep(350, 100);
		Beep(300, 200);
		Beep(350, 100);
		Beep(300, 200);
	}
#endif

	return 1;
}
#endif //#ifndef NO_HASHING
