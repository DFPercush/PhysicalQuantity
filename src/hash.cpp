

#include <PhysicalQuantity.h>

typedef PhysicalQuantity::CSubString csubstr;

#ifndef NO_HASHING

// If you change these values or add new units, rebuild hashTables.h
// In Visual Studio, a Rebuild All / Rebuild Solution will do this.
// The main() for this is in genhashtables.cpp
// See README.txt for instructions on adding new units.


// If you really need some serious hashing power from a proven algorithm....
//uint32_t murmur3_32(const unsigned char* key, size_t len, uint32_t seed);


// But in the name of speed, let's be a little simpler
size_t PhysicalQuantity::cstrHasherTiny::operator()(const CSubString& s) const
{
	//return murmur3_32((unsigned char*)s, strlen(s), 0x5f4d9a1b);
	size_t ret = 0;
	size_t c = (size_t)s[0];
	//for (int pos = 0; s[pos] != 0; pos++)
	int pos = 0;
	while (c != 0)
	{
		//ret += ((s[pos] - 64) * 0xB9CD) + s[pos];
		//ret += ((c) * seed) + c - 64;
		ret += ((c) * 8) + c - 64;
		pos++;
		c = (size_t)s[pos];
	}
	return ret;
}

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


#endif //#ifndef NO_HASHING
