
#include <PhysicalQuantity.h>

// These tables can go in ROM
// If you change these values or add new units,
// please run main() with 'hash' argument > (include)/hashTables.h
const int PhysicalQuantity::hashTableSize_UnitSymbols = 50;
const int PhysicalQuantity::hashTableSize_UnitLongNames = 50;
const int PhysicalQuantity::hashTableSize_PrefixSymbols = 50;
const int PhysicalQuantity::hashTableSize_PrefixLongNames = 50;


/*
// A bit much for such short strings, I think....
uint32_t murmur3_32(const unsigned char* key, size_t len, uint32_t seed)
{
	uint32_t h = seed;
	if (len > 3) {
		size_t i = len >> 2;
		do {
			uint32_t k;
			memcpy(&k, key, sizeof(uint32_t));
			key += sizeof(uint32_t);
			k *= 0xcc9e2d51;
			k = (k << 15) | (k >> 17);
			k *= 0x1b873593;
			h ^= k;
			h = (h << 13) | (h >> 19);
			h = h * 5 + 0xe6546b64;
		} while (--i);
	}
	if (len & 3) {
		size_t i = len & 3;
		uint32_t k = 0;
		do {
			k <<= 8;
			k |= key[i - 1];
		} while (--i);
		k *= 0xcc9e2d51;
		k = (k << 15) | (k >> 17);
		k *= 0x1b873593;
		h ^= k;
	}
	h ^= len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}
// */


//size_t PhysicalQuantity::cstrHasherTiny::operator()(const char* s) const

size_t PhysicalQuantity::cstrHasherTiny::operator()(const CSubString& s) const
{
	/*
	const int p = 31;
	//const int m = 1e9 + 9;
	const int m = 1003;
	size_t hash_value = 0;
	int p_pow = 1;
	char c;
	//for (char c : s) 
	for (int i = 0; s[i]; i++)
	{
	c = s[i];
	hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
	p_pow = (p_pow * p) % m;
	}
	return hash_value;
	*/

	size_t ret = 0;
	for (int pos = 0; s[pos] != 0; pos++)
	{
		ret += s[pos] - 64;
	}
	return ret;

	//return murmur3_32((unsigned char*)s, strlen(s), 0x5f4d9a1b);
}


#if !defined(NO_PRINTF) && !defined(NO_NEW)

void PhysicalQuantity_dumpHashTable(const void* pArray, const char * const* pcstrMember, int elementSize, int arrayLen, int hashTableSize, const char* arrayName)
{
	//auto a = (reinterpret_cast<const char**>((const void**)0));
	//int memberOfs = (int)(reinterpret_cast<const char*>(pcstrMember) - reinterpret_cast<const char*>(pArray));
	int memberOfs = (int)((const char*)(pcstrMember) - (const char*)(pArray));
	int* hashes = new int[arrayLen];
	int* counts = new int[hashTableSize];
	for (int i = 0; i < arrayLen; i++)
	{
		//hashes[i] = PhysicalQuantity::cstrHasherTiny{}(ElementStr(i)) % (hashTableSize);
		hashes[i] = PhysicalQuantity::cstrHasherTiny{}(
			//ElementStr(i)
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
	printf("struct %s_HashTableEntry_t { int bucketSize; int bucket[%d]; };\n%s_HashTableEntry_t %s_HashTable[] = \n{", 
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
			printf("-1");
			ib++;
		}
		printf("}}");
	}
	printf("\n};\n");
	delete[] hashes;
	delete[] counts;
}

/**************************************************************************************************
int PhysicalQuantity_dumpHashTable(int HashTableSize)
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
********************************************************************************/
#endif  // #if !defined(NO_PRINTF) && !defined(NO_NEW)
