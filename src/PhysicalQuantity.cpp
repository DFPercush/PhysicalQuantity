
// TODO: convert(), toString(const PreferredUnits& pu)

#include "../include/PhysicalQuantity.h"

using namespace std;


#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

const double PhysicalQuantity::Pi = 3.1415926535897932384626433832795;
PhysicalQuantity::UnitMismatchException::UnitMismatchException() : std::exception() {}
PhysicalQuantity::UnitMismatchException::UnitMismatchException(const char* message) : std::exception(message) {}
PhysicalQuantity::InvalidExpressionException::InvalidExpressionException() : std::exception() {}
PhysicalQuantity::InvalidExpressionException::InvalidExpressionException(const char* message) : std::exception(message) {}

std::unordered_map<const char*, int, PhysicalQuantity::cstrHasherTiny, PhysicalQuantity::cstrEq> PhysicalQuantity::UnitSymbolLookup;
std::unordered_map<const char*, int, PhysicalQuantity::cstrHasherTiny, PhysicalQuantity::cstrEq> PhysicalQuantity::UnitLongNameLookup;

//#define q PhysicalQuantity::QuantityType

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


// New units must be added in 2 places.
// Need a separate place for the arrays which hold the dimensional data about a unit, so we can have a const pointer to it,
// because the compiler can't convert from an initializer_list to a constant array pointer
//namespace PhysicalQuantityUnitDimensions
//{
//	typedef const PhysicalQuantity::UnitQty u;
//	typedef PhysicalQuantity::QuantityType q;
//
//	// QuantityType, exponent
//	u meter[] = {{q::DISTANCE, 1}};
//	u gram[] = {{q::MASS, 1}};
//	u second[] = {{q::TIME, 1}};
//	u hertz[] = {{q::TIME, -1}};
//	u minute[] = {{q::TIME, 1}};
//	u hour[] = {{q::TIME, 1}};
//	// Template:
//	// u [] = {{q::, }};
//}
////#define DeclareUnitDef(symbol, longName, factor) { #symbol, #longName, PhysicalQuantityUnitDimensions::longName, sizeof(PhysicalQuantityUnitDimensions::longName) / sizeof(PhysicalQuantity::UnitQty), factor}
const PhysicalQuantity::UnitDefinition PhysicalQuantity::KnownUnits[] = 
{
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
	{"K","kelvin",     { 0, 0, 0, 1, 0}, 0, 1.0 },
	{"C","coulomb",    { 0, 0, 0, 0, 1}, 0, 1.0 },


	/*Template

	//                   M  D Ti Te Ch

	{"", "",           { 0, 0, 0, 0, 0}, 0, 1.0},

	*/
};
const int PhysicalQuantity::KnownUnitsLength = sizeof(PhysicalQuantity::KnownUnits) / sizeof(PhysicalQuantity::UnitDefinition);

// Coulombs <==> electron charge
// 1C = 6.24150975*10^18e or 1e = 1.60217646*10^-19C

#undef q

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

PhysicalQuantity::PhysicalQuantity(double value_p, const string& units_p)
{
	init();
	value = value_p;
	double unitFactor;
	//units = parseUnits(units_p);
	parseUnits(units_p, dim, unitFactor);
	value *= unitFactor;
}

PhysicalQuantity::PhysicalQuantity(const string& str)
{
	init();
	parse(str);
}

//PhysicalQuantity::PhysicalQuantity(double value_p, const std::vector<PhysicalQuantity::UnitQty>& units_p)
//{
//	value = value_p;
//	units = units_p;
//}
//
//PhysicalQuantity::PhysicalQuantity(double value_p, std::vector<PhysicalQuantity::UnitQty>&& units_mv)
//{
//	value = value_p;
//	units = std::move(units_mv); // maybe redundant but eh, better to be clear, no run time cost
//}

//double PhysicalQuantity::convert(const std::vector<UnitQty> unit)
//{
//	throw exception("incomplete function");
//	return 0.0;
//}

double PhysicalQuantity::convert(const string& units)
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

//void PhysicalQuantity::parseUnits(std::string unitStr, std::vector<PhysicalQuantity::UnitQty>& unitsOut, double& factorOut)
void PhysicalQuantity::parseUnits(std::string unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], double& factorOut)
{
	//std::vector<UnitQty> ret;
	//unitsOut.clear();
	memset(unitsOut, 0, sizeof(unitsOut));
	factorOut = 1.0;
	char c = 0;
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
	double newValue = 0.0;
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
	double unitFactor;
	parseUnits(text.substr(firstNotNumber), dim, unitFactor);
	value *= unitFactor;
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

std::string PhysicalQuantity::toString(const PreferredUnits& pu)
{
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

PhysicalQuantity PhysicalQuantity::operator* (double rhs)
{
	PhysicalQuantity ret(*this);
	ret.value = value * rhs;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator/ (double rhs)
{
	PhysicalQuantity ret(*this);
	ret.value = value / rhs;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator+ (double rhs)
{
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		if (dim[i] != 0)
		{
			throw UnitMismatchException("");
		}
	}
	PhysicalQuantity ret(*this);
	ret.value = value + rhs;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator- (double rhs)
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

signed int PhysicalQuantity::findUnit(const string& name)
{
	signed int ret;
#ifdef LOW_RAM
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

size_t PhysicalQuantity::cstrHasherTiny::operator()(const char* s) const
{
	size_t ret = 0;
	unsigned int poly = 0xE53B;
	for (int pos = 0; s[pos] != 0; pos++)
	{
		if (ret & 0x80000000) { ret ^= poly; }
		ret = (ret << 1) ^ s[pos];
	}
	return ret;
}

bool PhysicalQuantity::operator==(const PhysicalQuantity& rhs)
{
	if (memcmp(dim, rhs.dim, sizeof(dim)) != 0) { return false; }
	return (value == rhs.value);
}


//============================================================================================
// Display and conversion stuff
PhysicalQuantity::PreferredUnits::PreferredUnits(const string& unitList)
{
	const char* pstr = unitList.c_str();
	int len = unitList.length();
	int pos = 0;
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
			if (unitIndex != -1) { prefs.push_back(unitIndex); }

			wordStart = wordEnd + 1;
			while (pstr[wordStart] == ' ') { wordStart++; }
			pos = wordStart;
		}
	}
}

bool PhysicalQuantity::cstrLess::operator()(const char* a, const char* b) const
{
	return (strcmp(a, b) < 0);
}

bool PhysicalQuantity::cstrEq::operator()(const char* a, const char* b) const
{
	return (strcmp(a, b) == 0);
}


#define DefineLiteral(symbol) PhysicalQuantity operator ""_##symbol (long double v) { PhysicalQuantity ret(v, #symbol); return ret; } PhysicalQuantity operator ""_##symbol (unsigned long long v) { PhysicalQuantity ret((double)v, #symbol); return ret; }
DefineLiteral(m)
DefineLiteral(g)
DefineLiteral(s)
DefineLiteral(Hz)
DefineLiteral(N)
DefineLiteral(J)


