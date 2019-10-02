#pragma once
#if !defined(NO_PRINTF) && !defined(NO_NEW)
#include <PhysicalQuantity.h>
//int PhysicalQuantity_dumpHashTable(int HashTableSize);
//void PhysicalQuantity_dumpHashTable(void* pArray, void* pcstrMember, int elementSize, int arrayLen, int hashTableSize, const char* arrayName);
void PhysicalQuantity_dumpHashTable(const void* pArray, const char * const* pcstrMember, int elementSize, int arrayLen, int hashTableSize, const char* arrayName);
#endif
