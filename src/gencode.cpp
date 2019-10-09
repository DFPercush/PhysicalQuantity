
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
	// TODO: Offsets
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
	header << "#if defined(CPP11)\n";

	for (int ui = 0; ui < PQ::KnownUnitsLength; ui++)
	{
		const PQ::UnitDefinition& u = PQ::KnownUnits[ui];
		if (u.flags & NOLITERAL) { continue; }
		//if (u.offset == 0.0)
		//if (ui >= PQ::KnownUnitsWithOffsetLength)
		//{
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
				<< u.factor << ","
				<< ((ui < PQ::KnownUnitsWithOffsetLength) ? PQ::KnownUnitsWithOffset[ui] : 0)
				<< ")" << endl;
		//}  // offset
	}

	header << "#else //#if defined(CPP11)\n";
	source << "#if !defined(CPP11)\n";

	for (int i = 0; i < PQ::KnownUnitsLength; i++)
	{
		const PQ::UnitDefinition& u = PQ::KnownUnits[i];
		if (u.flags & NOLITERAL) { continue; }
		//if (u.offset == 0.0)
		//if (i >= PQ::KnownUnitsWithOffsetLength)
		//{
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
		//}
	}

	header << "#endif //#else of #if defined(CPP11)\n\n//Units with additive offsets:\n";

	//for (int i = 0; i < PQ::KnownUnitsLength; i++)
	//{
	//	const PQ::UnitDefinition& u = PQ::KnownUnits[i];
	//	if (u.flags & NOLITERAL) { continue; }
	//	//if (u.offset != 0.0)
	//	if (i < PQ::KnownUnitsWithOffsetLength)
	//	{
	//		if (u.flags & NOPREFIX)
	//		{
	//			header << "DeclareLiteral(";
	//			source << "DefineLiteral(";
	//		}
	//		else
	//		{
	//			header << "DeclareLiteralWithPrefixes(";
	//			source << "DefineLiteralWithPrefixes(";
	//		}
	//		header << u.symbol << ")\n";
	//		source << u.symbol << ")\n";
	//	}
	//}

	header << "#endif //#ifndef NO_LITERALS\n";
	source << "#endif //!CPP11\n";
	source << "#endif //#ifndef NO_LITERALS\n";
}

#ifndef NO_HASHING

unsigned int dumpHashTable(string rootpath, int& out_bucketSize, float& out_coverage, const void* pArray, const char * const* pcstrMember, int elementSize, int arrayLen, int hashTableSize, const char* arrayName, const char* indexTypeName, size_t seed_p = 0, bool print = true)
{
	ofstream hh ;
	int coverCount = 0;
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
	//if (print) printf("struct %s_HashTableEntry_t { unsigned short bucketSize; %s bucket[%d]; };\n%s_HashTableEntry_t %s_HashTable[] = \n{", 
	//	arrayName, indexTypeName, maxCount, arrayName, arrayName);
	if (print)
	{
		//printf("struct %s_HashTableEntry_t { unsigned short bucketSize; %s bucket[%d]; };\n%s_HashTableEntry_t %s_HashTable[] = \n{", 
		//	arrayName, indexTypeName, maxCount, arrayName, arrayName);
		hh << "struct " << arrayName << "_HashTableEntry_t { PhysicalQuantity::bucketSize_t bucketSize; "
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
		if (counts[iTable] > 0) { coverCount++; }
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
	if (print) 
	{
		hh << "\n};\n"; 
		hh.close();
	}

	delete[] hashes;
	delete[] counts;
	out_bucketSize = maxCount;
	out_coverage = (float)coverCount / (float)hashTableSize;
	return maxCount;
}

/*****************************************************************************
unsigned int findSeed(string rootpath)
{
	unsigned int minSize = 0xFFFF;
	unsigned int minSeed = 0;
	time_t then, now;
	time(&then);
	int trash;
	float ftrash;
	for (unsigned int seed = 0; seed < 0xFFFF; seed++)
	{
		time(&now);
		if (difftime(now, then) > 5)
		{
			fprintf(stderr, "Trying %u\n", seed);
			then = now;
		}
		unsigned int r = dumpHashTable(rootpath, trash, ftrash, &PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].symbol, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitSymbols, "UnitSymbols", "PhysicalQuantity::unitIndex_t", seed, false);
		unsigned int r2 = dumpHashTable(rootpath, trash, ftrash, &PhysicalQuantity::KnownUnits[0], &PhysicalQuantity::KnownUnits[0].longName, sizeof(PhysicalQuantity::UnitDefinition), PhysicalQuantity::KnownUnitsLength, PhysicalQuantity::hashTableSize_UnitLongNames, "UnitLongNames", "PhysicalQuantity::unitIndex_t", seed,  false);
		if (r2 > r) { r = r2; }
		r2 = dumpHashTable(rootpath, trash, ftrash, &PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].symbol, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixSymbols, "PrefixSymbols", "PhysicalQuantity::prefixIndex_t", seed, false);
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
*****************************************************************************/


bool optimizeHashes(int maxTableSize, int maxSeed, int minBucketSize,
	int& out_tableSize_unitSymbols, int& out_seed_unitSymbols, int& out_unitSymbolsBucketSize, float& out_unitsymbolsCoverage,
	int& out_tableSize_unitLongNames, int& out_seed_unitLongNames, int& out_unitLongNamesBucketSize, float& out_unitLongNames_coverage,
	int& out_tableSize_prefixSymbols, int& out_seed_prefixSymbols, int& out_prefixSymbolsBucketSize, float& out_prefixSymbols_coverage
	)
{
	if (minBucketSize < 1) { minBucketSize = 1; }
	// results of each attempt
	int rBucketSize;
	float rCoverage;
	int bestTlen;
	int bestSeed;
	int bestBucketSize;
	float bestCoverage = 0;
	//float bestRank;
	time_t then, now;
	time(&now);
	then = now;
	cout << "Finding optimal hash parameters with\n"
		<< "  maxTableSize = " << maxTableSize 
		<< "  maxSeed = " << maxSeed << hex << "(0x" << maxSeed  << ")" << dec
		<< "\n  This might take a while...";
	cout.flush();

	//--------------------------------------------------------------------
	// Unit symbols
	bestBucketSize = 9999;
	bestCoverage = 0;
	for (int tlen = 1; tlen < maxTableSize && bestBucketSize > minBucketSize; tlen++)
	{
		for (int seed = 0; seed <= maxSeed && bestBucketSize > minBucketSize; seed++)
		{
			time(&now);
			if (difftime(now, then) >= 10)
			{
				float progress = (1/3.0f) * ((((float)tlen / maxTableSize) * maxSeed) + maxSeed) / (float)(maxTableSize * maxSeed);
				cout << " ... " << setprecision(3) << (progress + (0/3.0f)) * 100 << "%";
				cout.flush();
				then = now;
			}
			dumpHashTable("", rBucketSize, rCoverage, 
				&PhysicalQuantity::KnownUnits[0], 
				&PhysicalQuantity::KnownUnits[0].symbol, 
				sizeof(PhysicalQuantity::UnitDefinition),
				PhysicalQuantity::KnownUnitsLength,
				tlen,
				"UnitSymbols", "PhysicalQuantity::unitIndex_t",
				seed, false);
			// if (rankHash(rBucketSize, tlen, rCoverage) < ...
			if (rBucketSize < bestBucketSize)
			{
				bestBucketSize = rBucketSize;
				bestCoverage = rCoverage;
				bestSeed = seed;
				bestTlen = tlen;
			}
			else if ((rBucketSize == bestBucketSize) && (rCoverage > bestCoverage))
			{
				bestBucketSize = rBucketSize;
				bestCoverage = rCoverage;
				bestSeed = seed;
				bestTlen = tlen;
			}
		}
	}
	out_tableSize_unitSymbols = bestTlen;
	out_seed_unitSymbols = bestSeed;
	out_unitSymbolsBucketSize = bestBucketSize;
	out_unitsymbolsCoverage = bestCoverage;

	//--------------------------------------------------------------------
	// Unit long names
	bestBucketSize = 9999;
	bestCoverage = 0;
	for (int tlen = 1; tlen < maxTableSize && bestBucketSize > minBucketSize; tlen++)
	{
		for (int seed = 0; seed <= maxSeed && bestBucketSize > minBucketSize; seed++)
		{
			time(&now);
			if (difftime(now, then) >= 10)
			{
				float progress = (1/3.0f) + (0.33f * ((((float)tlen / maxTableSize) * maxSeed) + maxSeed) / (float)(maxTableSize * maxSeed));
				cout << " ... " << setprecision(3) << (progress + (1/3.0f)) * 100 << "%";
				cout.flush();
				then = now;
			}
			dumpHashTable("", rBucketSize, rCoverage,
				&PhysicalQuantity::KnownUnits[0],
				&PhysicalQuantity::KnownUnits[0].longName,
				sizeof(PhysicalQuantity::UnitDefinition),
				PhysicalQuantity::KnownUnitsLength,
				tlen,
				"UnitLongNames", "PhysicalQuantity::unitIndex_t",
				seed, false);
			if (rBucketSize < bestBucketSize)
			{
				bestBucketSize = rBucketSize;
				bestCoverage = rCoverage;
				bestSeed = seed;
				bestTlen = tlen;
			}
			else if ((rBucketSize == bestBucketSize) && (rCoverage > bestCoverage))
			{
				bestBucketSize = rBucketSize;
				bestCoverage = rCoverage;
				bestSeed = seed;
				bestTlen = tlen;
			}
		}
	}
	out_tableSize_unitLongNames = bestTlen;
	out_seed_unitLongNames = bestSeed;
	out_unitLongNamesBucketSize = bestBucketSize;
	out_unitLongNames_coverage = bestCoverage;

	//--------------------------------------------------------------------
	// Prefix symbols
	bestBucketSize = 9999;
	bestCoverage = 0;
	for (int tlen = 1; tlen < maxTableSize && bestBucketSize > minBucketSize; tlen++)
	{
		for (int seed = 0; seed <= maxSeed && bestBucketSize > minBucketSize; seed++)
		{
			time(&now);
			if (difftime(now, then) >= 10)
			{
				float progress = (2/3.0f) + ((1/3.0f) * ((((float)tlen / maxTableSize) * maxSeed) + maxSeed) / (float)(maxTableSize * maxSeed));
				cout << " ... " << setprecision(3) << (progress + (2/3.0f)) * 100 << "%";
				cout.flush();
				then = now;
			}
			dumpHashTable("", rBucketSize, rCoverage,
				&PhysicalQuantity::KnownPrefixes[0],
				&PhysicalQuantity::KnownPrefixes[0].symbol,
				sizeof(PhysicalQuantity::Prefix),
				PhysicalQuantity::KnownPrefixesLength,
				tlen,
				"PrefixSymbols", "PhysicalQuantity::prefixIndex_t",
				seed, false);
			if (rBucketSize < bestBucketSize)
			{
				bestBucketSize = rBucketSize;
				bestCoverage = rCoverage;
				bestSeed = seed;
				bestTlen = tlen;
			}
			else if ((rBucketSize == bestBucketSize) && (rCoverage > bestCoverage))
			{
				bestBucketSize = rBucketSize;
				bestCoverage = rCoverage;
				bestSeed = seed;
				bestTlen = tlen;
			}
		}
	}
	out_tableSize_prefixSymbols = bestTlen;
	out_seed_prefixSymbols = bestSeed;
	out_prefixSymbolsBucketSize = bestBucketSize;
	out_prefixSymbols_coverage = bestCoverage;

	cout << "\n  Optimization finished!\n";
	cout.flush();
	return true;
}

#endif //#ifndef NO_HASHING

void showinfo(string rootpath = "")
{
	cout << "gencode info:\n\n  PQ_HEADER_OPTIONS = 0x" << hex << PQ_HEADER_OPTIONS;
	if (PQHeaderOptionsMatch)
	{
		cout << " OK\n";
	}
	else
	{
		cout << " BAD (lib 0x" << hex << PhysicalQuantity::compiledHeaderOptions << ")\n";
	}
	cout << dec << endl;

	size_t totalRom = 0;
	size_t tmp;
	cout << "  sizeof(PQ) = " << sizeof(PQ) << endl;
	cout << "  sizeof(PQ::CSubString) = " << sizeof(PQ::CSubString) << endl;
	cout << "  sizeof(PQ::UnitList) = " << sizeof(PQ::UnitList) << endl;
	cout <<"  sizeof(PQ::UnitListBase::UnitPref) = " << sizeof(PhysicalQuantity::UnitListBase::UnitPref) << endl;
	cout << endl;

	tmp = (6 * sizeof(int)) + 
		//hashTableSize_UnitSymbols;
		//hashTableSize_UnitLongNames;
		//hashTableSize_PrefixSymbols;
		//hashTableSeed_UnitSymbols;
		//hashTableSeed_UnitLongNames;
		//hashTableSeed_PrefixSymbols;	
		+ sizeof(PQ::defaultHashSeed             )
		+ sizeof(PQ::compiledHeaderOptions       )
		+ sizeof(PQ::KnownUnitsLength            )
		+ sizeof(PQ::KnownUnitsWithOffsetLength  )
		+ sizeof(PQ::KnownPrefixesLength         )
		+ sizeof(PQ::dekaIndex                   );
	cout << "  static const scalars: " << tmp << " bytes\n\n";
	totalRom += tmp;

	cout << "  sizeof(PQ::UnitDefinition) = " << sizeof(PQ::UnitDefinition) << endl;
	tmp = PQ::KnownUnitsLength * sizeof(PQ::UnitDefinition);
	totalRom += tmp;
	cout <<	"  PQ::KnownUnits[" << PQ::KnownUnitsLength << "]: " << tmp << " bytes\n";
		//<< " (" << PQ::KnownUnitsLength << " elements)\n";
	size_t uslen = 0;
	for (int iu = 0; iu < PQ::KnownUnitsLength;iu++)
	{
		// +1 for null terminator
		uslen += strlen(PQ::KnownUnits[iu].symbol) + sizeof(char);
		uslen += strlen(PQ::KnownUnits[iu].longName) + sizeof(char);
	}
	totalRom += uslen;
	cout << "  Strings in KnownUnits[]: " << uslen << " bytes\n";
	cout << endl;

	cout << "  sizeof(PQ::Prefix) = " << sizeof(PQ::Prefix) << endl;
	tmp = sizeof(PQ::Prefix) * PQ::KnownPrefixesLength;
	totalRom += tmp;
	cout << "  PQ::KnownPrefixes[" << PQ::KnownPrefixesLength << "]: " << tmp << " bytes\n";
		//<< " (" << PQ::KnownPrefixesLength << " elements)\n";
	size_t pslen = 0;
	for (int ip = 0; ip < PQ::KnownPrefixesLength; ip++)
	{
		pslen += strlen(PQ::KnownPrefixes[ip].symbol) + sizeof(char);
		pslen += strlen(PQ::KnownPrefixes[ip].longName) + sizeof(char);
	}
	totalRom += pslen;
	cout << "  Strings in KnownPrefixes[]: " << pslen << " bytes\n";
	cout << endl;

#ifndef NO_HASHING
	cout << "  PQ::defaultHashSeed = " << PhysicalQuantity::defaultHashSeed << endl;
	cout << "  sizeof(PQ::bucketSize_t) = " << sizeof(PQ::bucketSize_t) << endl;
	int iTrash;
	float fTrash;

	// Read back the hash params from auto generated source
	ifstream hashParamsFileIn;
	size_t unitSymbolsSize = PQ::default_hashTableSize_UnitSymbols;
	size_t unitLongNamesSize = PQ::default_hashTableSize_UnitLongNames;
	size_t prefixSymbolsSize = PQ::default_hashTableSize_PrefixSymbols;
	size_t unitSymbolsSeed = PQ::defaultHashSeed;
	size_t unitLongNamesSeed = PQ::defaultHashSeed;
	size_t prefixSymbolsSeed = PQ::defaultHashSeed;
	hashParamsFileIn.open(rootpath + "src/hashParams.acpp");
	if (hashParamsFileIn.is_open())
	{
		string line;
		while (!hashParamsFileIn.eof())
		{
			getline(hashParamsFileIn, line);
			if (line.find("hashTableSize_UnitSymbols"  ) != string::npos) { unitSymbolsSize    = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
			if (line.find("hashTableSize_UnitLongNames") != string::npos) { unitLongNamesSize  = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
			if (line.find("hashTableSize_PrefixSymbols") != string::npos) { prefixSymbolsSize  = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
			if (line.find("hashTableSeed_UnitSymbols"  ) != string::npos) { unitSymbolsSeed    = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
			if (line.find("hashTableSeed_UnitLongNames") != string::npos) { unitLongNamesSeed  = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
			if (line.find("hashTableSeed_PrefixSymbols") != string::npos) { prefixSymbolsSeed  = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
		}
		hashParamsFileIn.close();
	}
	else
	{
		cout << "  *** Warning: Can not read hash parameters from src/hashParams.acpp. Using defaults.\n";
	}

	int us = dumpHashTable("", iTrash, fTrash, 
		&PhysicalQuantity::KnownUnits[0],
		&PhysicalQuantity::KnownUnits[0].symbol,
		sizeof(PhysicalQuantity::UnitDefinition),
		PhysicalQuantity::KnownUnitsLength,
		(int)unitSymbolsSize,
		"UnitSymbols", "PhysicalQuantity::unitIndex_t",
		unitSymbolsSeed, false);
	int ul = dumpHashTable("", iTrash, fTrash,
		&PhysicalQuantity::KnownUnits[0],
		&PhysicalQuantity::KnownUnits[0].longName,
		sizeof(PhysicalQuantity::UnitDefinition),
		PhysicalQuantity::KnownUnitsLength,
		(int)unitLongNamesSize,
		"UnitLongNames", "PhysicalQuantity::unitIndex_t",
		unitLongNamesSeed, false);
	int ps = dumpHashTable("", iTrash, fTrash,
		&PhysicalQuantity::KnownPrefixes[0],
		&PhysicalQuantity::KnownPrefixes[0].symbol,
		sizeof(PhysicalQuantity::Prefix),
		PhysicalQuantity::KnownPrefixesLength,
		(int)prefixSymbolsSize,
		"PrefixSymbols", "PhysicalQuantity::prefixIndex_t",
		prefixSymbolsSeed, false);

	tmp = (sizeof(PQ::bucketSize_t) + (sizeof(PQ::prefixIndex_t) * ps)) * prefixSymbolsSize;
	totalRom += tmp;
	cout << "  PrefixSymbols_HashTable[" << prefixSymbolsSize << "]: " 
			<< "bucketSize " << ps << ", "
			<< tmp << " bytes*\n";

	tmp = (sizeof(PQ::bucketSize_t) + (sizeof(PQ::unitIndex_t) * us)) * unitSymbolsSize;
	totalRom += tmp;
	cout << "  UnitSymbols_HashTable[" << unitSymbolsSize << "]: "
		<< "bucketSize " << us << ", "
		<< tmp << " bytes*\n";

	tmp = (sizeof(PQ::bucketSize_t) + (sizeof(PQ::unitIndex_t) * ul)) * unitLongNamesSize;
	totalRom += tmp;
	cout << "  UnitLongNames_HashTable[" << unitLongNamesSize << "]: "
		<< "bucketSize " << ul << ", "
		<< tmp << " bytes*\n";

	cout << "    * Padding may introduce inaccuracies. gencode does not have a compiled version.\n";
	cout << endl;
#endif //#ifndef NO_HASHING


	cout << "  Total ROM data: " << totalRom << " bytes\n";
	cout << endl;
}


int main(int argc, char** argv)
{
	int optMaxTableSize = PQ::KnownUnitsLength * 2;
	int optMaxSeed = 0xFF;
	int optMinBucketSize = 2;
	bool beep = false;
	bool writeFiles = true;

#ifdef NO_HASHING
	cout << " * gencode: NO_HASHING is defined.\n";
#endif
#ifdef NO_LITERALS
	cout << " * gencode: NO_LITERALS is defined.\n";
#endif
	
	if (!PQHeaderOptionsMatch)
	{
		// This really shouldn't happen because gencode should build from source, 
		// not link against the final library object.
		fprintf(stderr, "gencode: Error, lib code was compiled with different header options!");
		return 1;
	}
	cout << dec;


	//-------------------------------------------------------
	// Parse command line
	if (argc <= 1)
	{
		cout << "gencode options:\n\n";
		cout << "generate --rootpath [path]    Writes headers and source files.\n";
		cout << endl;
		cout << "optimize                      Finds optimal hash parameters by brute force.\n";
		cout << "   --max-seed [n]             Sets scanning range of seeds\n";
		cout << "   --max-table-size [n]       Largest acceptable hash table length.\n";
		cout << "                              Actual size will multiply by max bucket size.\n";
		cout << "   --dry-run                  Do not write any files.\n";
		cout << endl;
		cout << "info                          Shows struct size and memory info.\n";
		cout << endl;
		//cout << "  find-seed (deprecated)      finds optimal hash seed of default sizes\n";
		//cout << endl;

#ifdef _WIN32
		cout << ", --beep";
#endif
		cout << endl;
		return 1;
	}
	string rootpath;
	bool generate = false;
	bool info = false;
	bool runOptimize = false;
	for (int iArg = 1; iArg < argc; iArg++)
	{
		if (!strcmp(argv[iArg], "--beep")) { beep = true; }
		//else if (!strcmp(argv[iArg], "find-seed"))
		//{
		//	printf("0x%x\n", findSeed(rootpath));
		//	return 0;
		//}
		else if (!strcmp(argv[iArg], "--rootpath"))
		{
			iArg++;
			rootpath = argv[iArg];
		}
		else if (!strcmp(argv[iArg], "generate"))
		{
			generate = true;
		}
		else if (!strcmp(argv[iArg], "info"))
		{
			info = true;
		}
		else if (!strcmp(argv[iArg], "optimize"))
		{
			runOptimize = true;
		}
		else if (!strcmp(argv[iArg], "--max-seed"))
		{
			iArg++;
			optMaxSeed = atoi(argv[iArg]);
		}
		else if (!strcmp(argv[iArg], "--max-table-size"))
		{
			iArg++;
			optMaxTableSize = atoi(argv[iArg]);
		}
		else if (!strcmp(argv[iArg], "--min-bucket-size"))
		{
			iArg++;
			optMinBucketSize = atoi(argv[iArg]);
		}
		else if (!strcmp(argv[iArg], "--dry-run"))
		{
			writeFiles = false;
		}

		else
		{
			fprintf(stderr, " * gencode: Unknown command: %s\n", argv[iArg]);
			return 1;
		}
	}
	// End parse command line
	//-------------------------------------------------------


	if (rootpath.length() == 0)
	{
		cout << " * Warning: --rootpath is null.\n";
	}
	else if (rootpath[rootpath.length()] != '/' && rootpath[rootpath.length()] != '\\')
	{
		rootpath += "/";
	}


	if (info) { showinfo(); }

	if (!generate) { writeFiles = false; }

	int ret = 0;
	try
	{
#ifndef NO_HASHING
		int unitSymbolsSize =   (int)PQ::default_hashTableSize_UnitSymbols;
		int unitSymbolsSeed =   (int)PQ::defaultHashSeed;
		int unitLongNamesSize = (int)PQ::default_hashTableSize_UnitLongNames;
		int unitLongNamesSeed = (int)PQ::defaultHashSeed;
		int prefixSymbolsSize = (int)PQ::default_hashTableSize_PrefixSymbols;
		int prefixSymbolsSeed = (int)PQ::defaultHashSeed;
		int unitSymbolsBucketSize = 0;
		int unitLongNamesBucketSize = 0;
		int prefixSymbolsBucketSize = 0;
		float unitSymbolsCoverage = 0;
		float unitLongNamesCoverage = 0;
		float prefixSymbolsCoverage = 0;

		ofstream hashParamsFile;
		if (writeFiles)
		{ 
			hashParamsFile.open(rootpath + "src/hashParams.acpp");
			if (!hashParamsFile.is_open())
			{
				throw runtime_error("Can not write src/hashParams.acpp");
			}
		}
		if (runOptimize)
		{
			if (!optimizeHashes(optMaxTableSize, optMaxSeed, optMinBucketSize,
				unitSymbolsSize, unitSymbolsSeed, unitSymbolsBucketSize, unitSymbolsCoverage,
				unitLongNamesSize, unitLongNamesSeed, unitLongNamesBucketSize, unitLongNamesCoverage,
				prefixSymbolsSize, prefixSymbolsSeed, prefixSymbolsBucketSize, prefixSymbolsCoverage))
			{
				throw runtime_error("optimize returned false");
			}
			cout.flush();
			printf("      UnitSymbols_HashTable[%d][%d] %.2g%% used, seed=%d\n", unitSymbolsSize, unitSymbolsBucketSize, unitSymbolsCoverage * 100, unitSymbolsSeed);
			printf("    UnitLongNames_HashTable[%d][%d] %.2g%% used, seed=%d\n", unitLongNamesSize, unitLongNamesBucketSize, unitLongNamesCoverage * 100, unitLongNamesSeed);
			printf("    PrefixSymbols_HashTable[%d][%d] %.2g%% used, seed=%d\n", prefixSymbolsSize, prefixSymbolsBucketSize, prefixSymbolsCoverage * 100, prefixSymbolsSeed);
			fflush(stdout);
		}

//		char unitSymbolsBucketSizeText[20];
//		char unitLongNamesBucketSizeText[20];
//		char prefixSymbolsBucketSizeText[20];
//		char unitSymbolsCoverageText[20];
//		char unitLongNamesCoverageText[20];
//		char prefixSymbolsCoverageText[20];
//
//#ifdef _MSC_VER
//#pragma warning(disable:4996)
//#endif
//		if (unitSymbolsBucketSize == 0) { strcpy(unitSymbolsBucketSizeText, "??"); }
//		else { sprintf(unitSymbolsBucketSizeText,   "%d", unitSymbolsBucketSize); }
//		if (unitLongNamesBucketSize == 0) { strcpy(unitLongNamesBucketSizeText, "??"); }
//		else { sprintf(unitLongNamesBucketSizeText, "%d", unitLongNamesBucketSize); }
//		if (prefixSymbolsBucketSize == 0) { strcpy(prefixSymbolsBucketSizeText, "??"); }
//		else { sprintf(prefixSymbolsBucketSizeText, "%d", prefixSymbolsBucketSize); }
//		sprintf(unitSymbolsCoverageText,   "%.2g", 100.0f * unitSymbolsCoverage);
//		sprintf(unitLongNamesCoverageText, "%.2g", 100.0f * unitLongNamesCoverage);
//		sprintf(prefixSymbolsCoverageText, "%.2g", 100.0f * prefixSymbolsCoverage);
//#ifdef _MSC_VER
//#pragma warning(default:4996)
//#endif
//		cout.flush();

		//cout << "  Hash table info:\n" << setprecision(3) //   (seed)[tableSize][bucketSize]\n"
		//	<< "    Unit symbols: seed " << unitSymbolsSeed << ", length " << unitSymbolsSize 
		//	<< ", " << ((unitSymbolsCoverage == 0) ? "??" : unitSymbolsCoverageText) << "% used"
		//	<< ", bucket length " << ((unitSymbolsBucketSize == 0) ? "??" : unitSymbolsBucketSizeText) 
		//	<< endl
		//
		//	<< "    Unit long names: seed " << unitLongNamesSeed << ", length " << unitLongNamesSize 
		//	<< ", " << ((unitLongNamesCoverage == 0) ? "??" : unitLongNamesCoverageText) << "% used"
		//	<< ", bucket length " << ((unitLongNamesBucketSize == 0) ? "??" : unitLongNamesBucketSizeText)
		//	<< endl
		//
		//	<< "    Prefix symbols: seed " << prefixSymbolsSeed << ", length " << prefixSymbolsSize 
		//	<< ", " << ((prefixSymbolsCoverage == 0) ? "??" : prefixSymbolsCoverageText) << "% used"
		//	<< ", bucket length " << ((prefixSymbolsBucketSize == 0) ? "??" : prefixSymbolsBucketSizeText)
		//	<< endl;

		if (writeFiles)
		{
			hashParamsFile << "#if !defined(NO_HASHING) && !defined(PQ_GENCODE)\n";
			hashParamsFile << "#include <PhysicalQuantity.h>\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSize_UnitSymbols = " << unitSymbolsSize << ";\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSize_UnitLongNames = " << unitLongNamesSize << ";\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSize_PrefixSymbols = " << prefixSymbolsSize << ";\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSeed_UnitSymbols = " << unitSymbolsSeed << ";\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSeed_UnitLongNames = " << unitLongNamesSeed << ";\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSeed_PrefixSymbols = " << prefixSymbolsSeed << ";\n";
			hashParamsFile << "#endif\n";
			hashParamsFile.close();

			remove((rootpath + "include/PhysicalQuantity/hashTables.ah").c_str());
			cout << "gencode: Generating hash tables...";
			// TODO: Optimize
			int iTrash;
			float fTrash;
			dumpHashTable(rootpath, iTrash, fTrash,
				&PhysicalQuantity::KnownUnits[0],
				&PhysicalQuantity::KnownUnits[0].symbol,
				sizeof(PhysicalQuantity::UnitDefinition),
				PhysicalQuantity::KnownUnitsLength,
				unitSymbolsSize,
				"UnitSymbols", "PhysicalQuantity::unitIndex_t",
				unitSymbolsSeed, writeFiles);
			dumpHashTable(rootpath, iTrash, fTrash,
				&PhysicalQuantity::KnownUnits[0],
				&PhysicalQuantity::KnownUnits[0].longName,
				sizeof(PhysicalQuantity::UnitDefinition),
				PhysicalQuantity::KnownUnitsLength,
				unitLongNamesSize,
				"UnitLongNames", "PhysicalQuantity::unitIndex_t",
				unitLongNamesSeed, writeFiles);
			dumpHashTable(rootpath, iTrash, fTrash,
				&PhysicalQuantity::KnownPrefixes[0],
				&PhysicalQuantity::KnownPrefixes[0].symbol,
				sizeof(PhysicalQuantity::Prefix),
				PhysicalQuantity::KnownPrefixesLength,
				prefixSymbolsSize,
				"PrefixSymbols", "PhysicalQuantity::prefixIndex_t",
				prefixSymbolsSeed, writeFiles);
			cout << " OK\n";
		}

		//dumpHashTable(&PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].longName, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixLongNames, "PrefixLongNames");
#endif //#ifndef NO_HASHING

#ifndef NO_LITERALS
		if (writeFiles)
		{
			cout << "gencode: Generating literals... ";
			dumpLiterals(rootpath);
			cout << " OK\n";
		}
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

