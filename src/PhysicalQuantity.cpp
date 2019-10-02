
// TODO: convert(), toString(const PreferredUnits& pu), NO_THROW

#include <PhysicalQuantity.h>
#include <PhysicalQuantity/hash.h>
#include <PhysicalQuantity/hashTables.h>
using namespace std;

//const int PhysicalQuantity::PreferredUnitsBase::staticStorageSize = 4;


#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

const PhysicalQuantity::num PhysicalQuantity::Pi = 3.1415926535897932384626433832795;


#ifndef LOW_RAM
std::unordered_map<const char*, int, PhysicalQuantity::cstrHasherTiny, PhysicalQuantity::cstrEq> PhysicalQuantity::UnitSymbolLookup;
std::unordered_map<const char*, int, PhysicalQuantity::cstrHasherTiny, PhysicalQuantity::cstrEq> PhysicalQuantity::UnitLongNameLookup;
#endif


#ifdef NO_THROW
void (*PhysicalQuantity::errorHandler)(void* userContext, ErrorCode e);
void* PhysicalQuantity::errorUserContext;
#else
PhysicalQuantity::UnitMismatchException::UnitMismatchException() : std::exception() {}
PhysicalQuantity::UnitMismatchException::UnitMismatchException(const char* message) : std::exception(message) {}
PhysicalQuantity::InvalidExpressionException::InvalidExpressionException() : std::exception() {}
PhysicalQuantity::InvalidExpressionException::InvalidExpressionException(const char* message) : std::exception(message) {}
#endif

const PhysicalQuantity::UnitDefinition PhysicalQuantity::KnownUnits[] = 
{
	/*Template
	//                   M  D Ti Te Ch
	{"", "",           { 0, 0, 0, 0, 0}, 0, 1.0},
	*/

	// symbol, longName, { MASS DISTANCE TIME TEMPERATURE CHARGE }, offset, factor
	//                   M  D Ti Te Ch
	{"m", "meter",     {+0,+1,+0,+0,+0}, 0, 1.0},
	{"g", "gram",      { 1, 0, 0, 0, 0}, 0, 0.001},
	{"s", "second",    { 0, 0, 1, 0, 0}, 0, 1.0},
	{"Hz", "hertz",    { 0, 0,-1, 0, 0}, 0, 1.0},
	{"J", "joule",     { 1, 2,-2, 0, 0}, 0, 1.0},
	{"N", "newton",    { 1, 1,-2, 0, 0}, 0, 1.0},
	{"lb", "pound",    { 1, 1,-2, 0, 0}, 0, 4.448221615260501},
	//                   M  D Ti Te Ch
	{"K","kelvin",     { 0, 0, 0, 1, 0}, 0, 1.0},
	{"degC","degreeC", { 0, 0, 0, 1, 0}, 273.15, 1.0},
	//{"degF","degreeF", { 0, 0, 0, 1, 0}, 304.15, 9.0/5.0},
	{"degF","degreeF", { 0, 0, 0, 1, 0}, 255.37222222222222222222222222222, 0.55555555555555555555555555555556},
	{"C","coulomb",    { 0, 0, 0, 0, 1}, 0, 1.0},
	{"A","ampere",    { 0, 0,-1, 0, 1}, 0, 1.0},

};
const int PhysicalQuantity::KnownUnitsLength = sizeof(PhysicalQuantity::KnownUnits) / sizeof(PhysicalQuantity::UnitDefinition);

#define DefineLiteral(symbol) PhysicalQuantity operator ""_##symbol (long double v) { PhysicalQuantity ret(v, #symbol); return ret; } PhysicalQuantity operator ""_##symbol (unsigned long long v) { PhysicalQuantity ret(static_cast<PhysicalQuantity::num>(v), #symbol); return ret; }
DefineLiteral(m)
DefineLiteral(g)
DefineLiteral(s)
DefineLiteral(Hz)
DefineLiteral(N)
DefineLiteral(J)
DefineLiteral(K)
DefineLiteral(C)

// Coulombs <==> electron charge
// 1C = 6.24150975*10^18e or 1e = 1.60217646*10^-19C


const PhysicalQuantity::Prefix PhysicalQuantity::KnownPrefixes[] =
{
	{"y", "yocto", 1e-24},
{"z", "zepto", 1e-21},
{"a", "atto", 1e-18},
{"f", "femto", 1e-15},
{"p", "pico", 1e-12},
{"n", "nano", 1e-9},
{"u", "micro", 1e-6},  // Should I try to make the source files utf-8 for greek letter mu? Ehhhhh...
{"m", "milli", 1e-3},
{"c", "centi", 1e-2},
{"d", "deci", 0.1},
{"da", "deka", 10.0},
{"h", "hecto", 100.0},
{"k", "kilo", 1000.0},
{"M", "mega", 1e6},
{"G", "giga", 1e9},
{"T", "tera", 1e12},
{"P", "peta", 1e15},
{"E", "exa", 1e18},
{"Z", "zetta", 1e21},
{"Y", "yotta", 1e24}
};
const int PhysicalQuantity::KnownPrefixesLength = sizeof(PhysicalQuantity::KnownPrefixes) / sizeof(PhysicalQuantity::Prefix);

void PhysicalQuantity::init()
{
	if (UnitSymbolLookup.size() == 0)
	{
		for (int i = 0; i < KnownUnitsLength; i++)
		{
			UnitSymbolLookup[KnownUnits[i].symbol] = i;
			UnitLongNameLookup[KnownUnits[i].longName] = i;
		}
	}
}

PhysicalQuantity::PhysicalQuantity()
{
	init();
	value = 0.0;
}


PhysicalQuantity::PhysicalQuantity(const PhysicalQuantity& copy)
{
	init();
	value = copy.value;
	//units = copy.units;
	memcpy(dim, copy.dim, sizeof(dim));
}

PhysicalQuantity::PhysicalQuantity(PhysicalQuantity&& move) noexcept
{
	init();
	value = move.value;
	//units = std::move(move.units);
	memcpy(dim, move.dim, sizeof(dim));
}

PhysicalQuantity::PhysicalQuantity(num value_p, const string& units_p)
{
	init();
	value = value_p;
	num unitFactor;
	num unitOfs;
	//units = parseUnits(units_p);
	parseUnits(units_p, dim, unitFactor, unitOfs);
	value *= unitFactor;
	value += unitOfs;
}

PhysicalQuantity::PhysicalQuantity(const string& str)
{
	init();
	parse(str);
}

//PhysicalQuantity::PhysicalQuantity(num value_p, const std::vector<PhysicalQuantity::UnitQty>& units_p)
//{
//	value = value_p;
//	units = units_p;
//}
//
//PhysicalQuantity::PhysicalQuantity(num value_p, std::vector<PhysicalQuantity::UnitQty>&& units_mv)
//{
//	value = value_p;
//	units = std::move(units_mv); // maybe redundant but eh, better to be clear, no run time cost
//}

//num PhysicalQuantity::convert(const std::vector<UnitQty> unit)
//{
//	throw exception("incomplete function");
//	return 0.0;
//}

PhysicalQuantity::num PhysicalQuantity::convert(const string& units)
{
	throw exception("incomplete function");
	return 0.0;
}

bool matchFirst(string search, string match)
{
	const char* sptr = search.c_str();
	int slen = search.length();
	const char* mptr = match.c_str();
	int mlen = match.length();
	if (mlen > slen) { return false; }
	for (int i = 0; i < mlen; i++)
	{
		if (search[i] != match[i]) { return false; }
	}
	return true;
}

bool matchLast(string search, string match)
{
	const char* sptr = search.c_str();
	int slen = search.length();
	const char* mptr = match.c_str();
	int mlen = match.length();
	if (mlen > slen) { return false; }
	for (int i = 0; i < mlen; i++)
	{
		if (search[slen - mlen + i] != match[i]) { return false; }
	}
	return true;
}

//void PhysicalQuantity::parseUnits(std::string unitStr, std::vector<PhysicalQuantity::UnitQty>& unitsOut, num& factorOut)
void PhysicalQuantity::parseUnits(std::string unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& ofsOut)
{
	//std::vector<UnitQty> ret;
	//unitsOut.clear();
	memset(unitsOut, 0, sizeof(unitsOut));
	factorOut = 1.0;
	char c = 0;
	num tempOfs = 0.0;
	const char* uptr = unitStr.c_str();
	int ulen = unitStr.length();
	if (ulen == 0) { return; }
	int wordStart = 0;
	int wordEnd = 0;
	string word;
	while (uptr[wordStart] == ' ' && wordStart < ulen) { wordStart++; }
	int foundPrefix, foundUnit;
	int iCaret;
	bool denom = false;
	string unitName, powerStr;
	int upow = 0;
	bool denomNext = false;
	for (int i = wordStart; i <= ulen; i++)
	{
		if (denomNext) { denom = true; }
		if (i < ulen) { c = uptr[i]; }
		if (c == ' ' || c == '*' || c == '/' || i == ulen) 
		{
			wordEnd = i;

			// Do stuff
			word = unitStr.substr(wordStart, wordEnd - wordStart);

			if (c == '/')
			{
				if (denom) { throw InvalidExpressionException("Only one '/' to indicate units denominator is allowed."); }
				denomNext = true;
				wordStart = i + 1;
				while (wordStart < ulen && uptr[wordStart] == ' ') { wordStart++; }
				i = wordStart - 1;
				continue;
			}

			if (word == "/")
			{
				if (denom) { throw InvalidExpressionException("Only one '/' to indicate units denominator is allowed."); }
				denom = true;
			}
			else if (word == "*") {	} // implied
			else if (word == "^")
			{
				throw InvalidExpressionException("No spaces between unit and power please... for now.");
			}
			else
			{
				upow = 1;
				iCaret = word.find_first_of('^');
				if (iCaret != string::npos)
				{
					unitName = word.substr(0, iCaret);
					powerStr = word.substr(iCaret + 1);
					if (powerStr.length() == 0)
					{
						throw InvalidExpressionException("Expected an integer after '^' for unit exponent");
					}
					if (powerStr.find_first_not_of("0123456789-+") != string::npos)
					{
						throw InvalidExpressionException("Units may only have integer exponents.");
					}
					upow = atoi(powerStr.c_str());
					//if (denom) { upow *= -1;} // handled in mulunit()
				}
				else
				{
					unitName = word;
					upow = 1;
				}

				foundPrefix = -1;
				foundUnit = findUnit(unitName);
				if (foundUnit == -1)
				{
					foundPrefix = -1;
					for (int iPrefix = 0; iPrefix < KnownPrefixesLength; iPrefix++)
					{
						if (matchFirst(unitName, KnownPrefixes[iPrefix].symbol))
						{
							foundPrefix = iPrefix;
							break;
						}
						if (matchFirst(unitName, KnownPrefixes[iPrefix].longName))
						{
							foundPrefix = iPrefix;
							break;
						}
					}
					if (foundPrefix != -1)
					{
						string mainUnit = unitName.substr(strlen(KnownPrefixes[foundPrefix].symbol));
						foundUnit = findUnit(mainUnit);
						if (foundUnit == -1)
						{
							mainUnit = unitName.substr(strlen(KnownPrefixes[foundPrefix].longName));
							//foundUnit
							foundUnit = findUnit(mainUnit);
						}
					}

					//foundUnit = -1;
					if (foundUnit == -1)
					{
						for (int iUnit = 0; iUnit < KnownUnitsLength; iUnit++)
						{
							if (matchLast(unitName, KnownUnits[iUnit].symbol))
							{
								foundUnit = iUnit;
								break;
							}
							if (matchLast(unitName, KnownUnits[iUnit].longName))
							{
								foundUnit = iUnit;
								break;
							}
						}
					}
				}

				if (foundUnit == -1)
				{
					throw InvalidExpressionException("Unknown unit");
				}
				
				if ((foundPrefix != -1) &&
					(foundUnit != -1) && 
					(strlen(KnownPrefixes[foundPrefix].symbol) > word.length() - strlen(KnownUnits[foundUnit].symbol))
					)
				{
					// Collision between unit and prefix, for example "m"
					foundPrefix = -1;
				}
				else if (foundPrefix >= 0)
				{
					factorOut *= KnownPrefixes[foundPrefix].factor;
				}


				if (KnownUnits[foundUnit].offset != 0.0)
				{
					if (tempOfs != 0.0)
					{
						throw InvalidExpressionException("Multiple units given which apply an offset (e.g. degrees F)");
					}
					if (upow < -1 || upow > 1)
					{
						throw InvalidExpressionException("Can not handle an offset unit with a power greater than 1. (e.g. degrees F squared)");
					}
					tempOfs = KnownUnits[foundUnit].offset;
				}
				factorOut *= KnownUnits[foundUnit].factor;
				mulUnit(unitsOut, KnownUnits[foundUnit], upow, denom);
				//UnitQty q;
				//for (int iQty = 0; iQty < KnownUnits[foundUnit].nqtys; iQty++)
				//{
				//	q.type = KnownUnits[foundUnit].qtys[iQty].type;
				//	
				//}
				//q.unitDefIndex = foundUnit;
				//q.type = KnownUnits[foundUnit].type;
				//q.power = upow;
				//unitsOut.push_back(q);

			} // Not a special word

			// Advance to next word
			wordStart = wordEnd;
			while (uptr[wordStart] == ' ' && wordStart < ulen) { wordStart++; }
		}
	}
	
	// Apply offset (mainly for temperature in degrees)
	// Unless it's something like degrees per second, basically some derived unit
	bool applyOfs = true;
	if (tempOfs != 0.0)
	{
		for (int iCheckDim = 0; iCheckDim < (int)QuantityType::ENUM_MAX; iCheckDim++)
		{
			if (unitsOut[iCheckDim] != 0 && iCheckDim != (int)QuantityType::TEMPERATURE)
			{
				//throw InvalidExpressionException("");
				applyOfs = false;
			}
		}
	}
	if (applyOfs)
	{
		ofsOut = tempOfs;
	}
}

//void PhysicalQuantity::mulUnit(std::vector<UnitQty>& out, const UnitDefinition& unit, signed int power, bool invert)
//{
//	bool found0 = false;
//	bool foundMatch = false;
//	if (invert) { power *= -1; }
//	for (unsigned int iq = 0; iq < unit.nqtys; iq++)
//	{
//		foundMatch = false;
//		for (unsigned int io = 0; io < out.size(); io++)
//		{
//			if (out[io].type == unit.qtys[iq].type)
//			{
//				foundMatch = true;
//				out[io].power += unit.qtys[iq].power * power;
//
//				//if (invert)
//				//{
//				//	out[io].power -= unit.qtys[iq].power * power;
//				//}
//				//else
//				//{
//				//	out[io].power += unit.qtys[iq].power * power;
//				//}
//			}
//		}
//		if (!foundMatch)
//		{
//			UnitQty q = unit.qtys[iq];
//			q.power *= power;
//			out.push_back(q);
//		}
//	}
//
//	// Scan for zero powers and delete
//	do
//	{
//		found0 = false;
//		//for (decltype(out)::iterator it = out.begin)
//		//for (auto it : out)
//		for (std::vector<UnitQty>::iterator it = out.begin(); it != out.end(); it++)
//		{
//			//if (it.power) {}
//			if (it->power == 0)
//			{
//				found0 = true;
//				out.erase(it);
//				break;
//			}
//		}
//	} while (found0);
//}


//void PhysicalQuantity::mulUnit(signed char(&unitsOut)[(int)QuantityType::ENUM_MAX], const UnitDefinition& unit, signed int power, bool invert = false)
void PhysicalQuantity::mulUnit(signed char(&unitsOut)[(int)QuantityType::ENUM_MAX], const UnitDefinition& unit, signed int power, bool invert)
{
	if (invert) { power *= -1; }
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		unitsOut[i] += power * unit.dim[i];
	}
}

void PhysicalQuantity::parse(string text)
{
	num newValue = 0.0;
	int firstNotSpace = text.find_first_not_of(" ");
	if (firstNotSpace > 0) { text = text.substr(firstNotSpace); }
	if (text.length() == 0) { throw InvalidExpressionException("Quantity can not be null."); }
	string word;
	int firstNotNumber = text.find_first_not_of("0123456789.-+");
	if (firstNotNumber == string::npos)
	{
		value = atof(text.c_str());
		//units.clear();
		memset(dim, 0, sizeof(dim));
		return;
	}
	string exponentChars = "0123456789+-";
	if  ((signed)text.length() >= firstNotNumber && 
		(text[firstNotNumber] == 'e' || text[firstNotNumber] == 'E') &&
		exponentChars.find_first_of(text[firstNotNumber]) != string::npos)
	{
		// continuing the number value part of the text, denoting an exponent of scientific notation
		firstNotNumber = text.find_first_not_of(exponentChars);
		if (firstNotNumber == string::npos)
		{
			value = atof(text.c_str());
			//units.clear();
			memset(dim, 0, sizeof(dim));
			return;
		}
	}
	
	value = atof(text.c_str());
	num unitFactor;
	num unitOfs;
	parseUnits(text.substr(firstNotNumber), dim, unitFactor, unitOfs);
	value *= unitFactor;
	value += unitOfs;
}

std::string PhysicalQuantity::toString()
{
	string ret;
	// TODO: human friendly readability

	char numbuf[100];
	sprintf(numbuf, "%g", value);
	ret = numbuf; 

	if (dim[(int)QuantityType::MASS] > 0) { ret += " kg"; }
	if (dim[(int)QuantityType::MASS] > 1) 
	{
		sprintf(numbuf, "%d", (static_cast<signed int>(dim[(int)QuantityType::MASS])));
		ret += "^";
		ret += numbuf;
	}
	if (dim[(int)QuantityType::DISTANCE] > 0) { ret += " m"; }
	if (dim[(int)QuantityType::DISTANCE] > 1) 
	{
		sprintf(numbuf, "%d", (static_cast<signed int>(dim[(int)QuantityType::DISTANCE])));
		ret += "^";
		ret += numbuf;
	}
	if (dim[(int)QuantityType::TIME] > 0) { ret += " s"; }
	if (dim[(int)QuantityType::TIME] > 1) 
	{
		sprintf(numbuf, "%d", (static_cast<signed int>(dim[(int)QuantityType::TIME])));
		ret += "^";
		ret += numbuf;
	}
	if (dim[(int)QuantityType::TEMPERATURE] > 0) { ret += " K"; }
	if (dim[(int)QuantityType::TEMPERATURE] > 1)
	{
		sprintf(numbuf, "%d", (static_cast<signed int>(dim[(int)(int)QuantityType::TEMPERATURE])));
		ret += "^";
		ret += numbuf;
	}
	if (dim[(int)QuantityType::CHARGE] > 0) { ret += " C"; }
	if (dim[(int)QuantityType::CHARGE] > 1) 
	{
		sprintf(numbuf, "%d", (static_cast<signed int>(dim[(int)QuantityType::CHARGE])));
		ret += "^";
		ret += numbuf;
	}


	bool hasDenominator = false;
	for (int iDim = 0; iDim < (int)QuantityType::ENUM_MAX; iDim++)
	{
		if (dim[iDim] < 0)
		{
			hasDenominator = true;
			break;
		}
	}


	if (!hasDenominator) return ret;
	ret += " /";


	if (dim[(int)QuantityType::MASS] < 0) { ret += " kg"; }
	if (dim[(int)QuantityType::MASS] < -1) 
	{
		sprintf(numbuf, "%d", (-1 * static_cast<signed int>(dim[(int)QuantityType::MASS])));
		ret += "^";
		ret += numbuf;
	}
	if (dim[(int)QuantityType::DISTANCE] < 0) { ret += " m"; }
	if (dim[(int)QuantityType::DISTANCE] < -1) 
	{
		sprintf(numbuf, "%d", (-1 * static_cast<signed int>(dim[(int)QuantityType::DISTANCE])));
		ret += "^";
		ret += numbuf;
	}
	if (dim[(int)QuantityType::TIME] < 0) { ret += " s"; }
	if (dim[(int)QuantityType::TIME] < -1) 
	{
		sprintf(numbuf, "%d", (-1 * static_cast<signed int>(dim[(int)QuantityType::TIME])));
		ret += "^";
		ret += numbuf;
	}
	if (dim[(int)QuantityType::TEMPERATURE] < 0) { ret += " K"; }
	if (dim[(int)QuantityType::TEMPERATURE] < -1)
	{
		sprintf(numbuf, "%d", (-1 * static_cast<signed int>(dim[(int)(int)QuantityType::TEMPERATURE])));
		ret += "^";
		ret += numbuf;
	}
	if (dim[(int)QuantityType::CHARGE] < 0) { ret += " C"; }
	if (dim[(int)QuantityType::CHARGE] < -1) 
	{
		sprintf(numbuf, "%d", (-1 * static_cast<signed int>(dim[(int)QuantityType::CHARGE])));
		ret += "^";
		ret += numbuf;
	}

	return ret;
}

string PhysicalQuantity::toString(const PreferredUnitsBase& pu)
{
	// TODO: this func
	string ret;
	
	throw exception("Incomplete function");
}

PhysicalQuantity PhysicalQuantity::operator* (const PhysicalQuantity& rhs)
{
	PhysicalQuantity ret;
	ret.value = value * rhs.value;
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		ret.dim[i] = dim[i] + rhs.dim[i];
	}
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator/ (const PhysicalQuantity& rhs)
{
	PhysicalQuantity ret;
	ret.value = value / rhs.value;
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		ret.dim[i] = dim[i] - rhs.dim[i];
	}
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator+ (const PhysicalQuantity& rhs)
{
	if (memcmp(dim, rhs.dim, sizeof(dim)) != 0)
	{
		throw UnitMismatchException("Trying to add dissimilar units.");
	}
	PhysicalQuantity ret(*this);
	ret.value = value + rhs.value;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator- (const PhysicalQuantity& rhs)
{
	if (memcmp(dim, rhs.dim, sizeof(dim)) != 0)
	{
		throw UnitMismatchException("Trying to add dissimilar units.");
	}
	PhysicalQuantity ret(*this);
	ret.value = value - rhs.value;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator* (num rhs)
{
	PhysicalQuantity ret(*this);
	ret.value = value * rhs;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator/ (num rhs)
{
	PhysicalQuantity ret(*this);
	ret.value = value / rhs;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator+ (num rhs)
{
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		if (dim[i] != 0)
		{
#ifdef NO_THROW
			errorHandler(errorUserContext, E_UNIT_MISMATCH);
#else
			throw UnitMismatchException("");
#endif
		}
	}
	PhysicalQuantity ret(*this);
	ret.value = value + rhs;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator- (num rhs)
{
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		if (dim[i] != 0)
		{
			throw UnitMismatchException("");
		}
	}
	PhysicalQuantity ret(*this);
	ret.value = value - rhs;
	return ret;
}

bool PhysicalQuantity::findUnit(const char* name, int& outUnitIndex, int& outPrefixIndex)
{
#if defined(LOW_RAM) || defined(NO_THROW)
#else
#endif
	return false;
}

signed int PhysicalQuantity::findUnit(const string& name)
{
	// TODO: convert away from string
	signed int ret;
#if defined(LOW_RAM) || defined(NO_THROW)
	for (int i = 0; i < KnownUnitsLength; i++)
	{
		//if (matchLast(name, KnownUnits[i].symbol) || matchLast(name, KnownUnits[i].longName))
		if (name == KnownUnits[i].symbol || name == KnownUnits[i].longName)
		{
			return i;
		}
	}
	return -1;
#else
	try
	{
		ret = UnitSymbolLookup.at(name.c_str());
	}
	catch (const std::out_of_range & ex)
	{
		// Funy stuff to squash a warning
		ret = (ex.what() ? -1 : -1);
	}
	if (ret == -1)
	{
		try
		{
			ret = UnitLongNameLookup.at(name.c_str());
		}
		catch (const std::out_of_range & ex)
		{
			// Funy stuff to squash a warning
			ret = (ex.what() ? -1 : -1);
		}
	}
	return ret;
#endif

}

bool PhysicalQuantity::operator==(const PhysicalQuantity& rhs)
{
	if (memcmp(dim, rhs.dim, sizeof(dim)) != 0) { return false; }
	return (value == rhs.value);
}


//============================================================================================
// Display and conversion stuff
PhysicalQuantity::PreferredUnits_dynamic::PreferredUnits_dynamic(const string& unitList)
{
	const char* pstr = unitList.c_str();
	int len = unitList.length();
	int pos = 0;
	unitIndeces = new int[4]; //staticStorage;
	int storageCapacity = 4; // staticStorageSize;
	count_ = 0;
	std::pair<int, int> a;
	int wordStart = 0;
	int wordEnd = 0;
	string word;
	while (pstr[wordStart] == ' ') { wordStart++; }
	//while ( < len && pstr[wordStart] != 0)
	char c;
	int unitIndex;
	for (pos = wordStart; pos <= len; pos++)
	{
		if (pos < len) { c = pstr[pos]; }
		if (pos >= len || c == ' ')
		{
			wordEnd = pos;
			word = unitList.substr(wordStart, wordEnd - wordStart);

			unitIndex = PhysicalQuantity::findUnit(word);
			if (unitIndex != -1)
			{
				//prefs.push_back(unitIndex); 
				if (count_ >= storageCapacity)
				{
					int* tmpArray = new int[int(storageCapacity * 1.5) + 1];
					memcpy(tmpArray, unitIndeces, sizeof(int) * count_);
					unitIndeces = tmpArray;
				}
				unitIndeces[count_] = unitIndex;
				count_++;
			}

			wordStart = wordEnd + 1;
			while (pstr[wordStart] == ' ') { wordStart++; }
			pos = wordStart;
		}
	}
}
PhysicalQuantity::PreferredUnits_dynamic::~PreferredUnits_dynamic()
{
	//if (unitIndeces != staticStorage) { delete [] unitIndeces; }
	delete [] unitIndeces;
}

PhysicalQuantity::PreferredUnits_static::PreferredUnits_static(const char* unitList, int* storage, size_t storageSizeBytes)
{
	count_ = 0;
	unitIndeces = storage;
	int capacity = storageSizeBytes / sizeof(int);
	// TODO:
	//findUnit(unitList)
}

const PhysicalQuantity::UnitDefinition& PhysicalQuantity::PreferredUnitsBase::operator[] (int i)
{
	return KnownUnits[unitIndeces[i]];
}

bool PhysicalQuantity::cstrLess::operator()(const char* a, const char* b) const
{
	return (strcmp(a, b) < 0);
}

bool PhysicalQuantity::cstrEq::operator()(const char* a, const char* b) const
{
	return (strcmp(a, b) == 0);
}

CSubString::CSubString(const char* str_arg, size_t start_arg, size_t len_arg)
{
	str = str_arg;
	start = start_arg;
	end = start + len_arg;
}

bool CSubString::operator== (const char* cmp)
{
	for (int i = start; (i < end) && (*cmp != 0) && (str[i] != 0); i++)
	{
		if (str[i] != *cmp) { return false; }
		if (str[i] == 0) { return true; }
		if (*cmp == 0) { return false; }
		cmp++;
	}
	return true;
}

bool CSubString::begins(const char* test)
{
	for (int i = start; i < end; i++)
	{
		if (str[i] != *test) { return false; }
		if (*test == 0) { return false; }
		test++;
	}
	return true;
}

bool CSubString::ends(const char* test)
{
	int tlen = strlen(test);
	if (tlen < (end - start)) { return false; }
	int ti = tlen - (end - start);
	for (int i = start; i < end; i++)
	{
		if (str[i] != test[ti]) { return false; }
		if (str[i] == 0) { return true; }
		ti++;
	}
	return true;
}

bool CSubString::copy(char* buf, int size)
{
	int len = end - start;
	if (size <= len) { return false; }
	memcpy(buf, &str[start], len);
	buf[len] = 0;
	return true;
}

int CSubString::at(const char* test, int startArg)
{
	bool match;
	int mylen = end - start;
	for (int iStart = startArg; test[iStart] != 0; iStart++)
	{
		if (test[iStart] == 0) { return -1; }
		if (test[iStart] == str[start])
		{
			match = true;
			for (int i = 0; i < mylen; i++)
			{
				if (test[iStart + i] != str[start + i])
				{
					match = false;
					break;
				}
				if (str[start + i] == 0) { return iStart; }
				if (test[iStart + i] == 0) { return -1; }
			}
			if (match) { return iStart; }
		}
	}
	return -1;
}
