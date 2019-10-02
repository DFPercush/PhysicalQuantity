
#include <PhysicalQuantity.h>

size_t PhysicalQuantity::cstrHasherTiny::operator()(const char* s) const
{
	size_t ret = 0;
	//unsigned int poly = 0xE5; // 0xE53B; // not a real poly, just doing pseudo crc stuff
	for (int pos = 0; s[pos] != 0; pos++)
	{
		//if (ret & 0x80000000) { ret ^= poly; }
		//if (ret & 0x80) { ret ^= poly; }
		//ret = (ret << 1) ^ s[pos];
		//if (ret & 0x100) { ret = (ret ^ 1) & 0xFF; }
		ret += s[pos] - 64; //32;
	}
	return ret;
}

#ifndef NO_PRINTF
int PhysicalQuantity_buildHashTable(int HashTableSize)
{
	//const int HashTableSize = 0x20;
	int* unitSymbolHashes = new int[PhysicalQuantity::KnownUnitsLength];
	int* counts = new int[HashTableSize];
	for (int i = 0; i < PhysicalQuantity::KnownUnitsLength; i++)
	{
		//if (i > 0) { printf(", "); }
		//printf("%u", PhysicalQuantity::cstrHasherTiny{}(PhysicalQuantity::KnownUnits[i].symbol));
		unitSymbolHashes[i] = PhysicalQuantity::cstrHasherTiny{}(PhysicalQuantity::KnownUnits[i].symbol) % (HashTableSize);
	}

	int maxCount = 0;
	int thiscount;
	for (int icount = 0; icount < HashTableSize; icount++)
	{
		thiscount = 0;
		for (int iu = 0; iu < PhysicalQuantity::KnownUnitsLength; iu++)
		{
			if (unitSymbolHashes[iu] == icount) { thiscount++;}
		}
		counts[icount] = thiscount;
		if (thiscount > maxCount) maxCount = thiscount;
	}

	printf("struct UnitSymbolHashTableEntry_t { int bucketSize; int bucket[%d]; };\n UnitSymbolHashTableEntry_t UnitSymbolHashTable[] = \n{", maxCount);
	int ib;
	for (int iTable = 0; iTable < HashTableSize; iTable++)
	{
		ib = 0;
		if (iTable != 0) { printf(","); }
		printf("\n{%u,{", counts[iTable]);
		for (int iu = 0; iu < PhysicalQuantity::KnownUnitsLength; iu++)
		{
			if ((unitSymbolHashes[iu]) == iTable)
			{
				if (ib != 0) { printf(","); }
				printf("%u", iu);
				ib++;
			}
		}
		while (ib < maxCount)
		{
			if (ib != 0) { printf(","); }
			printf("-1");
			ib++;
		}
		printf("}}");
	}
	printf("\n};\n");
	delete[] unitSymbolHashes;
	delete[] counts;
	return 0;
}
#endif
