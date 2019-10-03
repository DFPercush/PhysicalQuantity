

#include <PhysicalQuantity.h>

#ifndef NO_HASHING

// These tables can go in ROM
// If you change these values or add new units,
// please run main() with 'hash' argument > (include)/hashTables.h
// See README.txt for instructions on adding new units.

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

size_t PhysicalQuantity::cstrHasherTiny::operator()(const CSubString& s) const
{
	//return murmur3_32((unsigned char*)s, strlen(s), 0x5f4d9a1b);
	size_t ret = 0;
	for (int pos = 0; s[pos] != 0; pos++)
	{
		ret += s[pos] - 64;
	}
	return ret;
}


#ifndef NO_PRINTF

void PhysicalQuantity_dumpHashTable(const void* pArray, const char * const* pcstrMember, int elementSize, int arrayLen, int hashTableSize, const char* arrayName)
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

#endif  //#ifndef NO_PRINTF

#endif //#ifndef NO_HASHING
