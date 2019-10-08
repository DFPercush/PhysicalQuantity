
// Note: Make sure PQ_GENCODE is defined in project settings / 
// preprocessor options, or as a compiler argument,
// or you will get errors about undeclared identifiers *_HashTable
// It's not enough to declare it here. Some source .cpp files also need it.

#include <PhysicalQuantity.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>
using namespace std;


typedef PhysicalQuantity PQ;
typedef PhysicalQuantity::CSubString csubstr;

#if defined(_MSC_VER) && defined(BEEP_IF_HASH_TABLES_REBUILT)
#include <Windows.h>
#endif


void dumpLiterals(string rootpath)
{
	ofstream header;
	ofstream source;
	if (rootpath[rootpath.length()] != '/' && rootpath[rootpath.length()] != '\\')
	{
		rootpath += "/";
	}
	header.open(rootpath + "include/PhysicalQuantity/literals.ah");
	if (!header.is_open()) { throw runtime_error("Could not open include/PhysicalQuantity/literals.ah"); }
	source.open(rootpath + "src/literals.acpp");
	if (!source.is_open())
	{
		header.close();
		throw runtime_error("Could not open src/literals.acpp");
	}

	source << "#ifndef NO_LITERALS\n";
	source << "#include <PhysicalQuantity.h>\n";

	header << "#ifndef NO_LITERALS\n";
	header << "#if defined(CPP11) && !defined(NO_INLINE)\n";

	for (int ui = 0; ui < PQ::KnownUnitsLength; ui++)
	{
		const PQ::UnitDefinition& u = PQ::KnownUnits[ui];
		if (u.flags & NOLITERAL) { continue; }
		if (u.offset == 0.0)
		{
			if (u.flags & NOPREFIX)
			{
				header << "CxLiteral(";
			}
			else
			{
				header << "CxLiteralWithPrefixes(";
			}
			header << u.symbol << ",";
			for (int di = 0; di < (int)PQ::QuantityType::ENUM_MAX; di++)
			{
				//fprintf(header, "%d,", u.dim[di]);
				header << (signed int)u.dim[di] << ",";
			}
			//fprintf(header, "%")
			header << setprecision(std::numeric_limits<long double>::digits10 + 1)
				<< u.factor << ")" << endl;
		}
	}

	header << "#else //#if defined(CPP11) && !defined(NO_INLINE)\n";
	source << "#if !defined(CPP11) || defined(NO_INLINE)\n";

	for (int i = 0; i < PQ::KnownUnitsLength; i++)
	{
		const PQ::UnitDefinition& u = PQ::KnownUnits[i];
		if (u.flags & NOLITERAL) { continue; }
		if (u.offset == 0.0)
		{
			if (u.flags & NOPREFIX)
			{
				header << "DeclareLiteral(";
				source << "DefineLiteral(";
			}
			else
			{
				header << "DeclareLiteralWithPrefixes(";
				source << "DefineLiteralWithPrefixes(";
			}
			header << u.symbol << ")\n";
			source << u.symbol << ")\n";
		}
	}

	header << "#endif //#else of #if defined(CPP11) && !defined(NO_INLINE)\n\n//Units with additive offsets:\n";

	for (int i = 0; i < PQ::KnownUnitsLength; i++)
	{
		const PQ::UnitDefinition& u = PQ::KnownUnits[i];
		if (u.flags & NOLITERAL) { continue; }
		if (u.offset != 0.0)
		{
			if (u.flags & NOPREFIX)
			{
				header << "DeclareLiteral(";
				source << "DefineLiteral(";
			}
			else
			{
				header << "DeclareLiteralWithPrefixes(";
				source << "DefineLiteralWithPrefixes(";
			}
			header << u.symbol << ")\n";
			source << u.symbol << ")\n";
		}
	}

	header << "#endif //#ifndef NO_LITERALS\n";
	source << "#endif //!CPP11 || NO_INLINE\n";
	source << "#endif //#ifndef NO_LITERALS\n";
}

#ifndef NO_HASHING

unsigned int dumpHashTable(string rootpath, const void* pArray, const char * const* pcstrMember, int elementSize, int arrayLen, int hashTableSize, const char* arrayName, const char* indexTypeName, size_t seed_p = 0, bool print = true)
{
	ofstream hh ;
	if (rootpath[rootpath.length()] != '/' && rootpath[rootpath.length()] != '\\')
	{
		rootpath += "/";
	}
	if (print)
	{
		hh.open(rootpath + "include/PhysicalQuantity/hashTables.ah", std::ofstream::app);
		if (!hh.is_open()) { throw runtime_error("Could not open include/PhysicalQuantity/hashTables.ah"); }
	}
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
	//if (print) printf("struct %s_HashTableEntry_t { unsigned int bucketSize; %s bucket[%d]; };\n%s_HashTableEntry_t %s_HashTable[] = \n{", 
	//	arrayName, indexTypeName, maxCount, arrayName, arrayName);
	if (print)
	{
		//printf("struct %s_HashTableEntry_t { unsigned int bucketSize; %s bucket[%d]; };\n%s_HashTableEntry_t %s_HashTable[] = \n{", 
		//	arrayName, indexTypeName, maxCount, arrayName, arrayName);
		hh << "struct " << arrayName << "_HashTableEntry_t { unsigned int bucketSize; "
			<< indexTypeName << " bucket[" << maxCount << "]; };\n" << arrayName
			<< "_HashTableEntry_t " << arrayName << "_HashTable[] = \n{\n";
	}
	int ib;
	for (int iTable = 0; iTable < hashTableSize; iTable++)
	{
		ib = 0;
		//if (iTable != 0) { if (print) printf(","); }
		if ((iTable != 0) && print) { hh << ","; }
		//if (print) printf("\n{%u,{", counts[iTable]);
		if (print) { hh << "\n{" << counts[iTable] << ",{"; }
		for (int iu = 0; iu < arrayLen; iu++)
		{
			if ((hashes[iu]) == iTable)
			{
				//if (ib != 0) { if (print) printf(","); }
				if ((ib != 0) && (print)) { hh << ","; }
				//if (print) printf("%u", iu);
				if (print) { hh << iu; }
				ib++;
			}
		}
		while (ib < maxCount)
		{
			//if (ib != 0) { if (print) printf(","); }
			if ((ib != 0) && (print)) { hh << ","; }
			//if (print) printf("0");
			if (print) { hh << "0"; }
			ib++;
		}
		//if (print) printf("}}");
		if (print) { hh << "}}"; }
	}
	//if (print) printf("\n};\n");
	if (print) { hh << "\n};\n"; }

	delete[] hashes;
	delete[] counts;
	return maxCount;
}

unsigned int findSeed(string rootpath)
{
	unsigned int minSize = 0xFFFF;
	unsigned int minSeed = 0;
	time_t then, now;
	time(&then);
	for (unsigned int seed = 0; seed < 0xFFFF; seed++)
	{
		time(&now);
		if (difftime(now, then) > 5)
		{
			fprintf(stderr, "Trying %u\n", seed);
			then = now;
		}
		unsigned int r = dumpHashTable(rootpath, &PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].symbol, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitSymbols, "UnitSymbols", "PhysicalQuantity::unitIndex_t", seed, false);
		unsigned int r2 = dumpHashTable(rootpath, &PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].longName, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitLongNames, "UnitLongNames", "PhysicalQuantity::unitIndex_t", seed,  false);
		if (r2 > r) { r = r2; }
		r2 = dumpHashTable(rootpath, &PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].symbol, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixSymbols, "PrefixSymbols", "PhysicalQuantity::prefixIndex_t", seed, false);
		if (r2 > r) { r = r2; }
		//r2 = dumpHashTable(&PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].longName, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixLongNames, "PrefixLongNames", seed, false);
		//if (r2 > r) { r = r2; }
		if (r < minSize)
		{
			minSize = r;
			minSeed = seed;
		}
	}
	return minSeed;
}
#endif //#ifndef NO_HASHING

void showinfo()
{
	cout << "\tsizeof(PQ) = " << sizeof(PQ) << endl;
	cout << "\tsizeof(PQ::CSubString) = " << sizeof(PQ::CSubString) << endl;
	cout << "\tsizeof(PQ::UnitList) = " << sizeof(PQ::UnitList) << endl;
	cout <<"\tsizeof(PQ::UnitListBase::UnitPref) = " << sizeof(PhysicalQuantity::UnitListBase::UnitPref) << endl;
	cout << "\tsizeof(PQ::UnitDefinition) = " << sizeof(PQ::UnitDefinition) << endl;
	cout <<	"\tsizeof(PQ::KnownUnits) = " << PQ::KnownUnitsLength * sizeof(PQ::UnitDefinition) 
		<< " (" << PQ::KnownUnitsLength << " elements)\n";
	cout << "\tsizeof(PQ::Prefix) = " << sizeof(PQ::Prefix) << endl;
	cout << "\tsizeof(PQ::KnownPrefixes) = " << sizeof(PQ::Prefix) * PQ::KnownPrefixesLength 
		<< " (" << PQ::KnownPrefixesLength << " elements)\n";
#ifndef NO_HASHING
	cout << "\tPQ::defaultHashSeed = " << PhysicalQuantity::defaultHashSeed << endl;
	int us = dumpHashTable("", &PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].symbol, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitSymbols, "UnitSymbols", "PhysicalQuantity::unitIndex_t", PhysicalQuantity::defaultHashSeed, false);
	int ul = dumpHashTable("", &PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].longName, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitLongNames, "UnitLongNames", "PhysicalQuantity::unitIndex_t", PhysicalQuantity::defaultHashSeed, false);
	int ps = dumpHashTable("", &PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].symbol, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixSymbols, "PrefixSymbols", "PhysicalQuantity::prefixIndex_t", PhysicalQuantity::defaultHashSeed, false);
	cout << "\tPrefixSymbols_HashTable[" << PQ::hashTableSize_PrefixSymbols << "]: " 
	     << "bucketSize " << ps << ", "
		 << (sizeof(int) + (sizeof(PQ::prefixIndex_t) * ps)) * PQ::hashTableSize_PrefixSymbols << " bytes*\n";
	//cout << "\tPQ::hashTableSize_UnitSymbols = " << PQ::hashTableSize_UnitSymbols << endl;
	cout << "\tUnitSymbols_HashTable[" << PQ::hashTableSize_UnitSymbols << "]: "
		<< "bucketSize " << us << ", "
		<< (sizeof(int) + (sizeof(PQ::unitIndex_t) * us)) * PQ::hashTableSize_UnitSymbols << " bytes*\n";
	//cout << "\tPQ::hashTableSize_UnitLongNames = " << PQ::hashTableSize_UnitLongNames << endl;	
	cout << "\tUnitLongNames_HashTable[" << PQ::hashTableSize_UnitLongNames << "]: "
		<< "bucketSize " << ul << ", "
		<< (sizeof(int) + (sizeof(PQ::unitIndex_t) * ul)) * PQ::hashTableSize_UnitLongNames << " bytes*\n";
	cout << "\t  * Padding may introduce inaccuracies. gencode does not have a compiled version.\n";
	//UnitLongNames_HashTable
#endif //#ifndef NO_HASHING
}


int main(int argc, char** argv)
{
	bool beep = false;
#ifdef NO_HASHING
	cout << " * gencode: NO_HASHING is defined.\n";
#endif
#ifdef NO_LITERALS
	cout << " * gencode: NO_LITERALS is defined.\n";
#endif
	
	cout << "gencode info:\n\tPreprocessor options 0x" << hex << PQ_HEADER_OPTIONS;
	if (PQHeaderOptionsMatch)
	{
		cout << " OK\n";
	}
	else
	{
		cout << " BAD (lib 0x" << PhysicalQuantity::compiledHeaderOptions << ")\n";
		return 1;
	}

	cout << dec;
	if (argc < 1)
	{
		cout << "gencode options: generate, find-seed, info, --rootpath [path] --beep (windows)\n";
		return 1;
	}
	string rootpath;
	bool generate = false;
	bool info = false;
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--beep")) { beep = true; }
		else if (!strcmp(argv[i], "find-seed"))
		{
			printf("0x%x", findSeed(rootpath));
			return 0;
		}
		else if (!strcmp(argv[i], "--rootpath"))
		{
			i++;
			rootpath = argv[i];
		}
		else if (!strcmp(argv[i], "generate"))
		{
			generate = true;
		}
		else if (!strcmp(argv[i], "info"))
		{
			info = true;
		}
		else { fprintf(stderr, " * gencode: Unknown command: %s\n", argv[i]); }
	}

	if (info) { showinfo(); }

	if (!generate) { return 0; }
	int ret = 0;
	try
	{
#ifndef NO_HASHING
		if (rootpath.length() == 0) { throw runtime_error("--rootpath not set"); }
		if (rootpath[rootpath.length()] != '/' && rootpath[rootpath.length()] != '\\')
		{
			rootpath += "/";
		}
		remove((rootpath + "include/PhysicalQuantity/hashTables.ah").c_str());
		cout << "gencode: Generating hash tables...";
		dumpHashTable(rootpath, &PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].symbol, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitSymbols, "UnitSymbols", "PhysicalQuantity::unitIndex_t");
		dumpHashTable(rootpath, &PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].longName, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitLongNames, "UnitLongNames", "PhysicalQuantity::unitIndex_t");
		dumpHashTable(rootpath, &PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].symbol, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixSymbols, "PrefixSymbols", "PhysicalQuantity::prefixIndex_t");
		cout << " OK\n";
		//dumpHashTable(&PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].longName, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixLongNames, "PrefixLongNames");
#endif //#ifndef NO_HASHING

#ifndef NO_LITERALS
		cout << "gencode: Generating literals... ";
		dumpLiterals(rootpath);
		cout << " OK\n";
#endif
#if defined(_MSC_VER) && defined(BEEP_IF_HASH_TABLES_REBUILT)
		if (beep)
		{
			Beep(440, 150),
			Beep((int)(440 * (pow(2,(5.0/12)))), 150);
		}
#endif
	}
	catch (const std::runtime_error& e)
	{
		fprintf(stderr, "%s", e.what());
		ret = 1;
		if (beep)
		{
#if defined(_MSC_VER) && defined(BEEP_IF_HASH_TABLES_REBUILT)
			Beep(350, 100);
			Beep(300, 100);
			Beep(350, 100);
			Beep(300, 100);
			Beep(350, 100);
			Beep(300, 100);
#endif
		}
	}
	cout << "gencode: exit code " << ret << endl;
	return ret;
}

