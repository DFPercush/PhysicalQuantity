#pragma once
#ifndef NO_PRINTF
//#include <PhysicalQuantity.h>
//int PhysicalQuantity_dumpHashTable(int HashTableSize);
//void PhysicalQuantity_dumpHashTable(void* pArray, void* pcstrMember, int elementSize, int arrayLen, int hashTableSize, const char* arrayName);
void PhysicalQuantity_dumpHashTable(const void* pArray, const char * const* pcstrMember, int elementSize, int arrayLen, int hashTableSize, const char* arrayName);
#endif //#ifndef(NO_PRINTF)
