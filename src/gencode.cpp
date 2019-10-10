
// Note: PQ_GENCODE is defined in project settings /
// preprocessor options, or as a compiler argument in makefile for
// any source files built as a dependency of gencode.
// If this #define is missing, you will get errors about undeclared identifiers *_HashTable
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
#ifndef NO_TEXT
typedef PhysicalQuantity::CSubString csubstr;
#endif

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
	header << "#if defined(CPP11)\n";

	for (int ui = 0; ui < PQ::KnownUnitsLength; ui++)
	{
		const PQ::UnitDefinition& u = PQ::KnownUnits[ui];
		if (u.flags & NOLITERAL) { continue; }
		//if (u.offset == 0.0)
		//if (ui >= PQ::KnownUnitOffsetsLength)
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
				<< ((ui < PQ::KnownUnitOffsetsLength) ? PQ::KnownUnitOffsets[ui] : 0)
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
		//if (i >= PQ::KnownUnitOffsetsLength)
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
	//	if (i < PQ::KnownUnitOffsetsLength)
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

#ifndef NO_TEXT
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
	if (print)
	{
		hh << "struct " << arrayName << "_HashTableEntry_t { PhysicalQuantity::bucketSize_t bucketSize; "
			<< indexTypeName << " bucket[" << maxCount << "]; };\n" << arrayName
			<< "_HashTableEntry_t " << arrayName << "_HashTable[] = \n{\n";
	}
	int ib;
	for (int iTable = 0; iTable < hashTableSize; iTable++)
	{
		ib = 0;
		if ((iTable != 0) && print) { hh << ","; }
		if (counts[iTable] > 0) { coverCount++; }
		if (print) { hh << "\n{" << counts[iTable] << ",{"; }
		for (int iu = 0; iu < arrayLen; iu++)
		{
			if ((hashes[iu]) == iTable)
			{
				if ((ib != 0) && (print)) { hh << ","; }
				if (print) { hh << iu; }
				ib++;
			}
		}
		while (ib < maxCount)
		{
			if ((ib != 0) && (print)) { hh << ","; }
			if (print) { hh << "0"; }
			ib++;
		}
		if (print) { hh << "}}"; }
	}
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

bool optimizeHashes(int maxTableSize, int maxSeed, int minBucketSize,
	int& out_tableSize_unitSymbols, int& out_seed_unitSymbols, int& out_unitSymbolsBucketSize, float& out_unitsymbolsCoverage,
	int& out_tableSize_unitLongNames, int& out_seed_unitLongNames, int& out_unitLongNamesBucketSize, float& out_unitLongNames_coverage,
	int& out_tableSize_prefixSymbols, int& out_seed_prefixSymbols, int& out_prefixSymbolsBucketSize, float& out_prefixSymbols_coverage,
	int& out_tableSize_unitPlurals, int& out_seed_unitPlurals, int& out_unitPluralsBucketSize, float& out_unitPlurals_coverage
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
				float progress = (1/4.0f) * ((((float)tlen / maxTableSize) * maxSeed) + maxSeed) / (float)(maxTableSize * maxSeed);
				cout << " ... " << setprecision(3) << (progress + (0/4.0f)) * 100 << "%";
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
				float progress = (2/4.0f) + ((1/3.0f) * ((((float)tlen / maxTableSize) * maxSeed) + maxSeed) / (float)(maxTableSize * maxSeed));
				cout << " ... " << setprecision(3) << (progress + (2/4.0f)) * 100 << "%";
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

#ifndef NO_LONG_NAMES
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
				float progress = (1/4.0f) + (0.33f * ((((float)tlen / maxTableSize) * maxSeed) + maxSeed) / (float)(maxTableSize * maxSeed));
				cout << " ... " << setprecision(3) << (progress + (1/4.0f)) * 100 << "%";
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
	// Unit plural names
	bestBucketSize = 9999;
	bestCoverage = 0;
	for (int tlen = 1; tlen < maxTableSize && bestBucketSize > minBucketSize; tlen++)
	{
		for (int seed = 0; seed <= maxSeed && bestBucketSize > minBucketSize; seed++)
		{
			time(&now);
			if (difftime(now, then) >= 10)
			{
				float progress = (3/4.0f) + (0.33f * ((((float)tlen / maxTableSize) * maxSeed) + maxSeed) / (float)(maxTableSize * maxSeed));
				cout << " ... " << setprecision(3) << (progress + (3/4.0f)) * 100 << "%";
				cout.flush();
				then = now;
			}
			dumpHashTable("", rBucketSize, rCoverage,
				&PhysicalQuantity::KnownUnits[0],
				&PhysicalQuantity::KnownUnits[0].plural,
				sizeof(PhysicalQuantity::UnitDefinition),
				PhysicalQuantity::KnownUnitsLength,
				tlen,
				"UnitPlurals", "PhysicalQuantity::unitIndex_t",
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
	out_tableSize_unitPlurals = bestTlen;
	out_seed_unitPlurals = bestSeed;
	out_unitPluralsBucketSize = bestBucketSize;
	out_unitPlurals_coverage = bestCoverage;
#else //#ifndef NO_LONG_NAMES
	out_unitLongNamesBucketSize = 0;
	out_unitLongNames_coverage = 0;
	out_tableSize_unitLongNames = 0;
	out_seed_unitLongNames;
	out_unitPluralsBucketSize = 0;
	out_unitPlurals_coverage = 0;
	out_tableSize_unitPlurals = 0;
	out_seed_unitPlurals = 0;
#endif //#else of #ifndef NO_LONG_NAMES

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

	tmp = (7 * sizeof(int))
		// Leaving these comments here so I know they're accounted for in prev line.
		// They will be part of the compiled library but not present here.
		//hashTableSize_UnitSymbols;
		//hashTableSize_UnitLongNames;
		//hashTableSize_PrefixSymbols;
		//hashTableSeed_UnitSymbols;
		//hashTableSeed_UnitLongNames;
		//hashTableSeed_PrefixSymbols;	
		//hashTableSize_UnitPlurals;
		+ sizeof(PQ::defaultHashSeed         )
		+ sizeof(PQ::compiledHeaderOptions   )
		+ sizeof(PQ::KnownUnitsLength        )
		+ sizeof(PQ::KnownUnitOffsetsLength  )
		+ sizeof(PQ::KnownPrefixesLength     )
		+ sizeof(PQ::dekaIndex               )
		;
	cout << "  static const scalars: " << tmp << " bytes\n\n";
	totalRom += tmp;

	cout << "  sizeof(PQ::UnitDefinition) = " << sizeof(PQ::UnitDefinition) << endl;
	tmp = PQ::KnownUnitsLength * sizeof(PQ::UnitDefinition);
	totalRom += tmp;
	cout <<	"  PQ::KnownUnits[" << PQ::KnownUnitsLength << "]: " << tmp << " bytes\n";
	size_t uslen = 0;
	for (int iu = 0; iu < PQ::KnownUnitsLength;iu++)
	{
		// +1 for null terminator
		uslen += strlen(PQ::KnownUnits[iu].symbol) + sizeof(char);
#ifndef NO_LONG_NAMES
		uslen += strlen(PQ::KnownUnits[iu].longName) + sizeof(char);
		uslen += strlen(PQ::KnownUnits[iu].plural) + sizeof(char);
#endif
	}
	totalRom += uslen;
	cout << "  Strings in KnownUnits[]: " << uslen << " bytes\n";
	cout << endl;

	cout << "  sizeof(PQ::Prefix) = " << sizeof(PQ::Prefix) << endl;
	tmp = sizeof(PQ::Prefix) * PQ::KnownPrefixesLength;
	totalRom += tmp;
	cout << "  PQ::KnownPrefixes[" << PQ::KnownPrefixesLength << "]: " << tmp << " bytes\n";
	size_t pslen = 0;
	for (int ip = 0; ip < PQ::KnownPrefixesLength; ip++)
	{
		pslen += strlen(PQ::KnownPrefixes[ip].symbol) + sizeof(char);
#ifndef NO_LONG_NAMES
		pslen += strlen(PQ::KnownPrefixes[ip].longName) + sizeof(char);
#endif
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
	size_t unitPluralsSize = PQ::default_hashTableSize_UnitPlurals;
	size_t unitSymbolsSeed = PQ::defaultHashSeed;
	size_t unitLongNamesSeed = PQ::defaultHashSeed;
	size_t prefixSymbolsSeed = PQ::defaultHashSeed;
	size_t unitPluralsSeed = PQ::defaultHashSeed;
	hashParamsFileIn.open(rootpath + "src/hashParams.acpp");
	if (hashParamsFileIn.is_open())
	{
		string line;
		while (!hashParamsFileIn.eof())
		{
			getline(hashParamsFileIn, line);
			if (line.find("hashTableSize_UnitSymbols"  ) != string::npos) { unitSymbolsSize    = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
			else if (line.find("hashTableSize_UnitLongNames") != string::npos) { unitLongNamesSize  = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
			else if (line.find("hashTableSize_PrefixSymbols") != string::npos) { prefixSymbolsSize  = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
			else if (line.find("hashTableSeed_UnitSymbols"  ) != string::npos) { unitSymbolsSeed    = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
			else if (line.find("hashTableSeed_UnitLongNames") != string::npos) { unitLongNamesSeed  = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
			else if (line.find("hashTableSeed_PrefixSymbols") != string::npos) { prefixSymbolsSeed  = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
			else if (line.find("hashTableSeed_UnitPlurals") != string::npos) { unitPluralsSeed  = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
			else if (line.find("hashTableSize_UnitPlurals") != string::npos) { unitPluralsSize  = atoi(line.substr(line.find_first_of("=") + 1).c_str()); }
		}
		hashParamsFileIn.close();
	}
	else
	{
		cout << "  *** Warning: Can not read hash parameters from src/hashParams.acpp. Using defaults.\n";
	}

	int ps = dumpHashTable("", iTrash, fTrash,
		&PhysicalQuantity::KnownPrefixes[0],
		&PhysicalQuantity::KnownPrefixes[0].symbol,
		sizeof(PhysicalQuantity::Prefix),
		PhysicalQuantity::KnownPrefixesLength,
		(int)prefixSymbolsSize,
		"PrefixSymbols", "PhysicalQuantity::prefixIndex_t",
		prefixSymbolsSeed, false);
	int us = dumpHashTable("", iTrash, fTrash, 
		&PhysicalQuantity::KnownUnits[0],
		&PhysicalQuantity::KnownUnits[0].symbol,
		sizeof(PhysicalQuantity::UnitDefinition),
		PhysicalQuantity::KnownUnitsLength,
		(int)unitSymbolsSize,
		"UnitSymbols", "PhysicalQuantity::unitIndex_t",
		unitSymbolsSeed, false);

#ifndef NO_LONG_NAMES
	int ul = dumpHashTable("", iTrash, fTrash,
		&PhysicalQuantity::KnownUnits[0],
		&PhysicalQuantity::KnownUnits[0].longName,
		sizeof(PhysicalQuantity::UnitDefinition),
		PhysicalQuantity::KnownUnitsLength,
		(int)unitLongNamesSize,
		"UnitLongNames", "PhysicalQuantity::unitIndex_t",
		unitLongNamesSeed, false);

	int up = dumpHashTable("", iTrash, fTrash,
		&PhysicalQuantity::KnownUnits[0],
		&PhysicalQuantity::KnownUnits[0].plural,
		sizeof(PhysicalQuantity::UnitDefinition),
		PhysicalQuantity::KnownUnitsLength,
		(int)unitPluralsSize,
		"UnitPlurals", "PhysicalQuantity::unitIndex_t",
		unitPluralsSeed, false);
#endif //#ifndef NO_LONG_NAMES

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

#ifndef NO_LONG_NAMES
	tmp = (sizeof(PQ::bucketSize_t) + (sizeof(PQ::unitIndex_t) * ul)) * unitLongNamesSize;
	totalRom += tmp;
	cout << "  UnitLongNames_HashTable[" << unitLongNamesSize << "]: "
		<< "bucketSize " << ul << ", "
		<< tmp << " bytes*\n";

	tmp = (sizeof(PQ::bucketSize_t) + (sizeof(PQ::unitIndex_t) * up)) * unitPluralsSize;
	totalRom += tmp;
	cout << "  UnitLongNames_HashTable[" << unitPluralsSize << "]: "
		<< "bucketSize " << up << ", "
		<< tmp << " bytes*\n";
#endif // #ifndef NO_LONG_NAMES

	cout << "    * Padding may introduce inaccuracies. gencode does not have a compiled version.\n";
	cout << endl;
#endif //#ifndef NO_HASHING


	cout << "  Total ROM data: " << totalRom << " bytes\n";
	cout << endl;
}

#endif //#ifdef NO_TEXT


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

	int ret = 0;


	try
	{

#ifdef NO_TEXT
		if (generate && writeFiles)
		{
			string filepath;
			string couldnotopen = "Could not open ";
			ofstream hashTablesAh;
			filepath = rootpath + "include/PhysicalQuantity/hashTables.ah";
			hashTablesAh.open(filepath);
			if (hashTablesAh.is_open())
			{
				hashTablesAh << "// NO_TEXT is #defined.\n";
				hashTablesAh.close();
			}
			else 
			{
				ret = 1; 
				throw runtime_error((couldnotopen + filepath).c_str());
			}
			
			ofstream hashParamsAcpp;
			filepath = rootpath + "src/hashParams.acpp";
			hashParamsAcpp.open(filepath);
			if (hashParamsAcpp.is_open())
			{
				hashParamsAcpp << "// NO_TEXT is #defined.\n";
				hashParamsAcpp.close();
			}
			else
			{
				ret = 1;
				throw runtime_error((couldnotopen + filepath).c_str());
			}
		}
#else //#ifdef NO_TEXT

	if (info) { showinfo(); }

	if (!generate) { writeFiles = false; }

//#ifdef NO_TEXT
//	cout << "  NO_TEXT is defined, nothing to generate. Exiting.";
//	cout.flush();
//	return 0;
//#else

#ifndef NO_HASHING
		int unitSymbolsSize =   (int)PQ::default_hashTableSize_UnitSymbols;
		int unitSymbolsSeed =   (int)PQ::defaultHashSeed;
		int unitSymbolsBucketSize = 0;
		float unitSymbolsCoverage = 0;
//#ifndef NO_LONG_NAMES
		int unitLongNamesSize = (int)PQ::default_hashTableSize_UnitLongNames;
		int unitLongNamesSeed = (int)PQ::defaultHashSeed;
		int unitLongNamesBucketSize = 0;
		float unitLongNamesCoverage = 0;
		int unitPluralsSize   = (int)PQ::default_hashTableSize_UnitPlurals;
		int unitPluralsSeed   = (int)PQ::defaultHashSeed;
		int unitPluralsBucketSize = 0;
		float unitPluralsCoverage = 0;
//#endif //#ifndef NO_LONG_NAMES
		int prefixSymbolsSize = (int)PQ::default_hashTableSize_PrefixSymbols;
		int prefixSymbolsSeed = (int)PQ::defaultHashSeed;
		int prefixSymbolsBucketSize = 0;
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
				prefixSymbolsSize, prefixSymbolsSeed, prefixSymbolsBucketSize, prefixSymbolsCoverage,
				unitPluralsSize, unitPluralsSeed, unitPluralsBucketSize, unitPluralsCoverage))
			{
				throw runtime_error("optimize returned false");
			}
			cout.flush();
			printf("      UnitSymbols_HashTable[%d][%d] %.2g%% used, seed=%d\n", unitSymbolsSize, unitSymbolsBucketSize, unitSymbolsCoverage * 100, unitSymbolsSeed);
#ifndef NO_LONG_NAMES
			printf("    UnitLongNames_HashTable[%d][%d] %.2g%% used, seed=%d\n", unitLongNamesSize, unitLongNamesBucketSize, unitLongNamesCoverage * 100, unitLongNamesSeed);
			printf("      UnitPlurals_HashTable[%d][%d] %.2g%% used, seed=%d\n", unitPluralsSize, unitPluralsBucketSize, unitPluralsCoverage * 100, unitPluralsSeed);
#endif
			printf("    PrefixSymbols_HashTable[%d][%d] %.2g%% used, seed=%d\n", prefixSymbolsSize, prefixSymbolsBucketSize, prefixSymbolsCoverage * 100, prefixSymbolsSeed);
			fflush(stdout);
		}

		if (writeFiles)
		{
			hashParamsFile << "#include <PhysicalQuantity.h>\n";
			hashParamsFile << "#if !defined(NO_HASHING) && !defined(PQ_GENCODE) && !defined(NO_TEXT)\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSize_UnitSymbols = " << unitSymbolsSize << ";\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSeed_UnitSymbols = " << unitSymbolsSeed << ";\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSize_PrefixSymbols = " << prefixSymbolsSize << ";\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSeed_PrefixSymbols = " << prefixSymbolsSeed << ";\n";
#ifndef NO_LONG_NAMES
			hashParamsFile << "const int PhysicalQuantity::hashTableSize_UnitLongNames = " << unitLongNamesSize << ";\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSeed_UnitLongNames = " << unitLongNamesSeed << ";\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSize_UnitPlurals = " << unitPluralsSize << ";\n";
			hashParamsFile << "const int PhysicalQuantity::hashTableSeed_UnitPlurals = " << unitPluralsSeed << ";\n";
#endif
			hashParamsFile << "#endif\n";
			hashParamsFile.close();

			remove((rootpath + "include/PhysicalQuantity/hashTables.ah").c_str());
			cout << "gencode: Generating hash tables...";
			// TODO: Optimize
			int iTrash;
			float fTrash;
			dumpHashTable(rootpath, iTrash, fTrash,
				&PhysicalQuantity::KnownPrefixes[0],
				&PhysicalQuantity::KnownPrefixes[0].symbol,
				sizeof(PhysicalQuantity::Prefix),
				PhysicalQuantity::KnownPrefixesLength,
				prefixSymbolsSize,
				"PrefixSymbols", "PhysicalQuantity::prefixIndex_t",
				prefixSymbolsSeed, writeFiles);
			dumpHashTable(rootpath, iTrash, fTrash,
				&PhysicalQuantity::KnownUnits[0],
				&PhysicalQuantity::KnownUnits[0].symbol,
				sizeof(PhysicalQuantity::UnitDefinition),
				PhysicalQuantity::KnownUnitsLength,
				unitSymbolsSize,
				"UnitSymbols", "PhysicalQuantity::unitIndex_t",
				unitSymbolsSeed, writeFiles);
#ifndef NO_LONG_NAMES
			dumpHashTable(rootpath, iTrash, fTrash,
				&PhysicalQuantity::KnownUnits[0],
				&PhysicalQuantity::KnownUnits[0].longName,
				sizeof(PhysicalQuantity::UnitDefinition),
				PhysicalQuantity::KnownUnitsLength,
				unitLongNamesSize,
				"UnitLongNames", "PhysicalQuantity::unitIndex_t",
				unitLongNamesSeed, writeFiles);
			dumpHashTable(rootpath, iTrash, fTrash,
				&PhysicalQuantity::KnownUnits[0],
				&PhysicalQuantity::KnownUnits[0].plural,
				sizeof(PhysicalQuantity::UnitDefinition),
				PhysicalQuantity::KnownUnitsLength,
				unitPluralsSize,
				"UnitPlurals", "PhysicalQuantity::unitIndex_t",
				unitPluralsSeed, writeFiles);
#endif //#ifndef NO_LONG_NAMES
			cout << " OK\n";
		}

		//dumpHashTable(&PhysicalQuantity::KnownPrefixes[0], &PhysicalQuantity::KnownPrefixes[0].longName, sizeof(PhysicalQuantity::Prefix), PhysicalQuantity::KnownPrefixesLength, PhysicalQuantity::hashTableSize_PrefixLongNames, "PrefixLongNames");
#endif //#ifndef NO_HASHING
#endif //#else of #ifdef NO_TEXT
	

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

