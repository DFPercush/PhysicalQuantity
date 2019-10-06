
// Note: Make sure PQ_GENERATING_HASH_TABLES is defined in project settings / 
// preprocessor options, or as a compiler argument,
// or you will get errors about undeclared identifiers *_HashTable
// It's not enough to declare it here. Some source .cpp files also need it.

#include <PhysicalQuantity.h>
#include <stdio.h>
//#include <PhysicalQuantity/hash.h>

#ifndef NO_HASHING

typedef PhysicalQuantity::CSubString csubstr;

#if defined(_MSC_VER) && defined(BEEP_IF_HASH_TABLES_REBUILT)
#include <Windows.h>
#endif


void dumpHashTable(const void* pArray, const char * const* pcstrMember, int elementSize, int arrayLen, int hashTableSize, const char* arrayName)
{
	int memberOfs = (int)((const char*)(pcstrMember) - (const char*)(pArray));
	int* hashes = new int[arrayLen];
	int* counts = new int[hashTableSize];
	for (int i = 0; i < arrayLen; i++)
	{
		hashes[i] = PhysicalQuantity::cstrHasherTiny{}(
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
	printf("struct %s_HashTableEntry_t { unsigned int bucketSize; unsigned int bucket[%d]; };\n%s_HashTableEntry_t %s_HashTable[] = \n{", 
		arrayName, maxCount, arrayName, arrayName);
	int ib;
	for (int iTable = 0; iTable < hashTableSize; iTable++)
	{
		ib = 0;
		if (iTable != 0) { printf(","); }
		printf("\n{%u,{", counts[iTable]);
		for (int iu = 0; iu < arrayLen; iu++)
		{
			if ((hashes[iu]) == iTable)
			{
				if (ib != 0) { printf(","); }
				printf("%u", iu);
				ib++;
			}
		}
		while (ib < maxCount)
		{
			if (ib != 0) { printf(","); }
			printf("0");
			ib++;
		}
		printf("}}");
	}
	printf("\n};\n");
	delete[] hashes;
	delete[] counts;
}


int main()
{
	dumpHashTable(&PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].symbol, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitSymbols, "UnitSymbols");
	dumpHashTable(&PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].longName, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitLongNames, "UnitLongNames");
	dumpHashTable(&PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].symbol, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixSymbols, "PrefixSymbols");
	dumpHashTable(&PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].longName, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixLongNames, "PrefixLongNames");

#if defined(_MSC_VER) && defined(BEEP_IF_HASH_TABLES_REBUILT)
	Beep(440, 150),
	Beep((int)(440 * (pow(2,(5.0/12)))), 150);
#endif
	return 0;
}

#else //#ifndef NO_HASHING
int main()
{
	fprintf(stderr, "Warning: Compiled without hashing, I can do nothing!\n");
	return 1;
}
#endif //#ifndef NO_HASHING
