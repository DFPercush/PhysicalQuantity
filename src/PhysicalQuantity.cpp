/*
TODO: 
	. convert()
	. toString(const PreferredUnits& pu)
	. initializer_list ? {1.23, {0,1,0,0,0}}
	. constexpr
*/

#include <PhysicalQuantity.h>
#ifndef NO_HASHING
#include <PhysicalQuantity/hash.h>
#include <PhysicalQuantity/hashTables.h>
#endif //#ifndef NO_HASHING
using namespace std;

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

const PhysicalQuantity::num PhysicalQuantity::Pi = 3.1415926535897932384626433832795;
PhysicalQuantity::num PhysicalQuantity::equalityToleranceFactor = 1e-6;
const int PhysicalQuantity::compiledHeaderOptions = PQ_HEADER_OPTIONS;

#ifdef NO_THROW
void (*PhysicalQuantity::errorHandler)(void* userContext, ErrorCode e);
void* PhysicalQuantity::errorUserContext;
#else
PhysicalQuantity::UnitMismatchException::UnitMismatchException() : std::exception() {}
PhysicalQuantity::UnitMismatchException::UnitMismatchException(const char* message) : std::exception(message) {}
PhysicalQuantity::InvalidExpressionException::InvalidExpressionException() : std::exception() {}
PhysicalQuantity::InvalidExpressionException::InvalidExpressionException(const char* message) : std::exception(message) {}
PhysicalQuantity::HeaderConfigException::HeaderConfigException() : std::exception() {}
PhysicalQuantity::HeaderConfigException::HeaderConfigException(const char* message) : std::exception(message) {}

#endif

const PhysicalQuantity::UnitDefinition PhysicalQuantity::KnownUnits[] = {
	/*Template
	//                   M  D Ti Te Ch
	{"", "",           { 0, 0, 0, 0, 0}, 0, 1.0},
	*/

	// symbol, longName, { MASS DISTANCE TIME TEMPERATURE CHARGE }, offset, factor
	//                   M  D Ti Te Ch
	{"m", "meter",     {+0,+1,+0,+0,+0}, 0, 1.0},
	{"ang","angstrom", { 0, 1, 0, 0, 0}, 0, 1.0e-10},
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

#ifndef NO_LITERALS
// TODO: Also define with all prefixes?
//#define DefineLiteral(symbol) PhysicalQuantity operator ""_##symbol (long double v) { PhysicalQuantity ret(v, #symbol); return ret; } PhysicalQuantity operator ""_##symbol (unsigned long long v) { PhysicalQuantity ret(static_cast<PhysicalQuantity::num>(v), #symbol); return ret; }
#define DefineLiteral(symbol) PhysicalQuantity operator ""_##symbol (long double v) { return PhysicalQuantity((PhysicalQuantity::num)v, #symbol); } PhysicalQuantity operator ""_##symbol (unsigned long long v) { return PhysicalQuantity((PhysicalQuantity::num)(v), #symbol); }
DefineLiteral(m)
DefineLiteral(g)
DefineLiteral(s)
DefineLiteral(Hz)
DefineLiteral(N)
DefineLiteral(J)
DefineLiteral(K)
DefineLiteral(C)
DefineLiteral(ang)
#endif  // !NO_LITERALS

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
const int dekaIndex = 10; // If any more prefixes are added, this should be the index of {"da", "deka", 10}
                          // Used to optimize lookups because this is the only prefix longer than 1 char
                          // If that changes, might need to change findUnit()

void PhysicalQuantity::init()
{
	value = 0.0;
	memset(dim, 0, sizeof(dim));
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

PhysicalQuantity::PhysicalQuantity(num value_p, const char* units_p)
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

PhysicalQuantity::PhysicalQuantity(const char* str)
{
	init();
	parse(str);
}

PhysicalQuantity::PhysicalQuantity(PhysicalQuantity::num valueArg)
{
	value = valueArg;
	memset(dim, 0, sizeof(dim));
}

PhysicalQuantity& PhysicalQuantity::operator=(PhysicalQuantity::num valueArg)
{
	value = valueArg;
	memset(dim, 0, sizeof(dim));
	return *this;
}
PhysicalQuantity& PhysicalQuantity::operator=(const PhysicalQuantity& cp)
{
	value = cp.value;
	memcpy(dim, cp.dim, sizeof(dim));
	return *this;
}

PhysicalQuantity::num PhysicalQuantity::convert(const CSubString& units) const
{
	// TODO: here
#ifdef NO_THROW
	errorHandler(errorUserContext, E_LOGIC_ERROR);
#else
	throw exception("incomplete function");
#endif //#ifdef NO_THROW
	return 0.0;
}

bool PhysicalQuantity::isLikeQuantity(const PhysicalQuantity& rhs) const
{
	if (!memcmp(dim, rhs.dim, sizeof(dim))) { return true; }
	return false;
}

/**********************************************************
bool matchFirst(string search, string match)
{
	const char* sptr = search.c_str();
	int slen = (int)search.length();
	const char* mptr = match.c_str();
	int mlen = (int)match.length();
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
	int slen = (int)search.length();
	const char* mptr = match.c_str();
	int mlen = (int)match.length();
	if (mlen > slen) { return false; }
	for (int i = 0; i < mlen; i++)
	{
		if (search[slen - mlen + i] != match[i]) { return false; }
	}
	return true;
}
**********************************************************************/

void PhysicalQuantity::parseUnits(const CSubString& unitStr, signed char (&unitsOut_arg)[(int)QuantityType::ENUM_MAX], num& factorOut_arg, num& ofsOut_arg)
{
	signed char unitsOut[(int)QuantityType::ENUM_MAX];
	memset(unitsOut, 0, sizeof(unitsOut));
	num factorOut = 1.0;
	num ofsOut = 0.0;
	char c = 0;
	num tempOfs = 0.0;
	int ulen = unitStr.length();
	if (ulen == 0) 
	{
		factorOut_arg = factorOut;
		ofsOut_arg = ofsOut;
		memcpy(unitsOut_arg, unitsOut, sizeof(unitsOut));
		return; 
	}
	int wordStart = 0;
	int wordEnd = 0;
	csubstr word;
	while (unitStr[wordStart] == ' ' && wordStart < ulen) { wordStart++; }
	int foundPrefix = -1;
	int foundUnit = -1;
	int iCaret;
	bool denom = false;
	csubstr ssUnitName, powerStr;
	int upow = 0;
	bool denomNext = false;
	for (int i = wordStart; i <= ulen; i++)
	{
		if (denomNext) { denom = true; }
		if (i < ulen) { c = unitStr[i]; }
		if (c == ' ' || c == '*' || c == '/' || i == ulen) 
		{
			wordEnd = i;

			// Do stuff
			word = csubstr(unitStr, wordStart, wordEnd - wordStart);  //unitStr.substr(wordStart, wordEnd - wordStart);

			if (c == '/')
			{
				if (denom)
				{
#ifdef NO_THROW
					errorHandler(errorUserContext, E_INVALID_EXPRESSION);
					return;
#else
					throw InvalidExpressionException("Only one '/' to indicate units denominator is allowed."); 
#endif //#ifdef NO_THROW
				}
				denomNext = true;
				wordStart = i + 1;
				while (wordStart < ulen && unitStr[wordStart] == ' ') { wordStart++; }
				i = wordStart - 1;
				continue;
			}

			if (word == "/")
			{
				if (denom) 
				{
#ifdef NO_THROW
					errorHandler(errorUserContext, E_INVALID_EXPRESSION);
					return;
#else
					throw InvalidExpressionException("Only one '/' to indicate units denominator is allowed."); 
#endif //#ifdef NO_THROW
				}
				denom = true;
			}
			else if (word == "*") {	} // implied
			else if (word == "^")
			{
#ifdef NO_THROW
				errorHandler(errorUserContext, E_INVALID_EXPRESSION);
				return;
#else
				throw InvalidExpressionException("No spaces between unit and power please... for now.");
#endif //#ifdef NO_THROW
			}
			else
			{
				upow = 1;
				iCaret = (int)word.find_first_of("^");
				if (iCaret != -1)
				{
					ssUnitName = csubstr(word, 0, iCaret);  //word.substr(0, iCaret);
					powerStr = csubstr(word, iCaret + 1); //word.substr(iCaret + 1);
					if (powerStr.length() == 0)
					{
#ifdef NO_THROW
						errorHandler(errorUserContext, E_INVALID_EXPRESSION);
						return;
#else
						throw InvalidExpressionException("Expected an integer after '^' for unit exponent");
#endif
					}
					if (powerStr.find_first_not_of("0123456789-+") != -1)
					{
#ifdef NO_THROW
						errorHandler(errorUserContext, E_INVALID_EXPRESSION);
						return;
#else
						throw InvalidExpressionException("Units may only have integer exponents.");
#endif
					}
					upow = powerStr.atoi();
				}
				else
				{
					ssUnitName = word;
					upow = 1;
				}

				if (!findUnit(ssUnitName, foundUnit, foundPrefix))
				{
#ifdef NO_THROW
					errorHandler(errorUserContext, E_INVALID_EXPRESSION);
					return;
#else
					throw InvalidExpressionException();
#endif
				}
				
				if (foundPrefix >= 0)
				{
					factorOut *= KnownPrefixes[foundPrefix].factor;
				}
				if (KnownUnits[foundUnit].offset != 0.0)
				{
					if (tempOfs != 0.0)
					{
#ifdef NO_THROW
						errorHandler(errorUserContext, E_INVALID_EXPRESSION);
						return;
#else
						throw InvalidExpressionException("Multiple units given which apply an offset (e.g. degrees F)");
#endif
					}
					if (upow < -1 || upow > 1)
					{
#ifdef NO_THROW
						errorHandler(errorUserContext, E_INVALID_EXPRESSION);
						return;
#else
						throw InvalidExpressionException("Can not handle an offset unit with a power greater than 1. (e.g. degrees F squared)");
#endif
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
			while (unitStr[wordStart] == ' ' && wordStart < ulen) { wordStart++; }
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
	else
	{
		ofsOut = 0;
	}
	factorOut_arg = factorOut;
	ofsOut_arg = ofsOut;
}



/*****************************************
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
	int ulen = (int)unitStr.length();
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
				iCaret = (int)word.find_first_of('^');
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
**************************************************************/


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

void PhysicalQuantity::parse(const char* text_arg)
{
	CSubString text(text_arg);
	num newValue = 0.0;
	//char numbuf40[40];
	int firstNotSpace = (int)text.find_first_not_of(" ");
	if (firstNotSpace > 0) { text = text.substr(firstNotSpace); } //{ text = CSubString(text, firstNotSpace); } //
	if (text.length() == 0)
	{
#ifdef NO_THROW
		errorHandler(errorUserContext, E_INVALID_EXPRESSION);
		return;
#else
		throw InvalidExpressionException("Quantity can not be null."); 
#endif
	}
	//string word;
	//csubstr word;
	int firstNotNumber = (int)text.find_first_not_of("0123456789Ee.-+"); // added Ee
	if (firstNotNumber == -1)
	{
		value = text.atof(); //atof(text.c_str());
		//units.clear();
		memset(dim, 0, sizeof(dim));
		return;
	}
	
	////string exponentChars = "0123456789+-";
	//const char* exponentCharsStr = "0123456789+-";
	//CSubString exponentChars(exponentCharsStr);
	//if  ((signed)text.length() >= firstNotNumber && 
	//	(text[firstNotNumber] == 'e' || text[firstNotNumber] == 'E')  )//&&
	//	//exponentChars.find_first_of(text[firstNotNumber]) != -1)
	//{
	//	// continuing the number value part of the text, denoting an exponent of scientific notation
	//	firstNotNumber = (int)text.find_first_not_of("0123456789eE+-"); //(exponentChars);
	//	if (firstNotNumber == -1)
	//	{
	//		value = text.atof(); //atof(text.c_str());
	//		//units.clear();
	//		memset(dim, 0, sizeof(dim));
	//		return;
	//	}
	//}
	
	value = text.atof();
	num unitFactor = 1.0;
	num unitOfs = 0.0;
	parseUnits(CSubString(text, firstNotNumber), dim, unitFactor, unitOfs);
	value *= unitFactor;
	value += unitOfs;
}

bool PhysicalQuantity::sprint(char* buf, int size) const
{
	// TODO: Here
	return false;
}
bool PhysicalQuantity::sprint(char* buf, int size, const PreferredUnitsBase& pu) const
{
	// TODO: Here
	return false;
}

#ifndef NO_STD_STRING

std::string PhysicalQuantity::toString() const
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

string PhysicalQuantity::toString(const PreferredUnitsBase& pu) const
{
	// TODO: this func
	string ret;
	
#ifdef NO_THROW
	errorHandler(errorUserContext, E_LOGIC_ERROR);
	return "";
#else
	throw exception("Incomplete function");
#endif
}

#endif //#ifndef NO_STD_STRING

PhysicalQuantity PhysicalQuantity::operator* (const PhysicalQuantity& rhs) const
{
	PhysicalQuantity ret;
	ret.value = value * rhs.value;
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		ret.dim[i] = dim[i] + rhs.dim[i];
	}
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator/ (const PhysicalQuantity& rhs) const
{
	PhysicalQuantity ret;
	ret.value = value / rhs.value;
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		ret.dim[i] = dim[i] - rhs.dim[i];
	}
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator+ (const PhysicalQuantity& rhs) const
{
	if (memcmp(dim, rhs.dim, sizeof(dim)) != 0)
	{
#ifdef NO_THROW
		errorHandler(errorUserContext, E_UNIT_MISMATCH);
		return *this;
#else
		throw UnitMismatchException("Trying to add dissimilar units.");
#endif
	}
	PhysicalQuantity ret(*this);
	ret.value = value + rhs.value;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator- (const PhysicalQuantity& rhs) const
{
	if (memcmp(dim, rhs.dim, sizeof(dim)) != 0)
	{
#ifdef NO_THROW
		errorHandler(errorUserContext, E_UNIT_MISMATCH);
		return *this;
#else
		throw UnitMismatchException("Trying to add dissimilar units.");
#endif
	}
	PhysicalQuantity ret(*this);
	ret.value = value - rhs.value;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator* (num rhs) const
{
	PhysicalQuantity ret(*this);
	ret.value = value * rhs;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator/ (num rhs) const
{
	PhysicalQuantity ret(*this);
	ret.value = value / rhs;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator+ (num rhs) const
{
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		if (dim[i] != 0)
		{
#ifdef NO_THROW
			errorHandler(errorUserContext, E_UNIT_MISMATCH);
			return *this;
#else
			throw UnitMismatchException("");
#endif
		}
	}
	PhysicalQuantity ret(*this);
	ret.value = value + rhs;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator- (num rhs) const
{
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		if (dim[i] != 0)
		{
#ifdef NO_THROW
			errorHandler(errorUserContext, E_UNIT_MISMATCH);
			return *this;
#else
			throw UnitMismatchException("");
#endif
		}
	}
	PhysicalQuantity ret(*this);
	ret.value = value - rhs;
	return ret;
}

bool PhysicalQuantity::findUnit(CSubString name, int& outUnitIndex, int& outPrefixIndex)
{
	int iPrefix = -1;
	int iUnit = -1;
	const char* daStr = "da";
	int nlen = name.length(); // (int)strlen(name);
	//const char* possibleUnitPart;
	csubstr possibleUnitPart;
	int foundPrefixLen = 0;
	int foundUnitLen = 0;

#ifndef NO_HASHING
	static cstrHasherTiny hasher;
	int iBucket, hashRaw, hashSymbol, hashLongName;
	char firstLetter[2];

	if ((nlen > 2) && (name[0] == 'd') && (name[1] == 'a'))
	{
		//hashTableSize_UnitSymbols
		possibleUnitPart = csubstr(name, 2);
		hashRaw = (int)hasher(possibleUnitPart);
		hashSymbol = hashRaw % hashTableSize_UnitSymbols;
		for (iBucket = 0; iBucket < UnitSymbols_HashTable[hashSymbol].bucketSize; iBucket++)
		{
			//if (!strcmp(possibleUnitPart, KnownUnits[UnitSymbols_HashTable[hash].bucket[iBucket]].symbol))
			if (possibleUnitPart == KnownUnits[UnitSymbols_HashTable[hashSymbol].bucket[iBucket]].symbol)
			{
				outPrefixIndex = dekaIndex;
				outUnitIndex = UnitSymbols_HashTable[hashSymbol].bucket[iBucket];
				return true;
			}
		}
		hashLongName = hashRaw % hashTableSize_UnitLongNames;
		for (iBucket = 0; iBucket < UnitLongNames_HashTable[hashLongName].bucketSize; iBucket++)
		{
			if (possibleUnitPart == KnownUnits[UnitLongNames_HashTable[hashLongName].bucket[iBucket]].longName)
			{
				outPrefixIndex = dekaIndex;
				outUnitIndex = UnitLongNames_HashTable[hashLongName].bucket[iBucket];
				return true;
			}
		}
	}

	firstLetter[0] = name[0];
	firstLetter[1] = 0;
	hashSymbol = hasher(firstLetter) % hashTableSize_PrefixSymbols;
	for (iBucket = 0; iBucket < PrefixSymbols_HashTable[hashSymbol].bucketSize; iBucket++)
	{
		if (name[0] == KnownPrefixes[PrefixSymbols_HashTable[hashSymbol].bucket[iBucket]].symbol[0])
		{
			iPrefix = PrefixSymbols_HashTable[hashSymbol].bucket[iBucket];
			foundPrefixLen = (int)strlen(KnownPrefixes[iPrefix].symbol);
			break;
		}
	}


	//if (iPrefix != -1) { possibleUnitPart = csubstr(name, 1);} // name + 1; }
	//else { possibleUnitPart = name; }

	csubstr tryUnitNames[2];
	tryUnitNames[0] = csubstr(name, 1);
	tryUnitNames[1] = csubstr(name);
	int iTryUnitName = 0; // (iPrefix == -1) ? 1 : 0;

	// look up unit symbol
	while(iTryUnitName < 2)
	{
		hashRaw = (int)hasher(tryUnitNames[iTryUnitName]);
		hashSymbol = hashRaw % hashTableSize_UnitSymbols;
		for (iBucket = 0; iBucket < UnitSymbols_HashTable[hashSymbol].bucketSize; iBucket++)
		{
			//if (!strcmp(possibleUnitPart, KnownUnits[UnitSymbols_HashTable[hash].bucket[iBucket]].symbol))
			if (tryUnitNames[iTryUnitName] == KnownUnits[UnitSymbols_HashTable[hashSymbol].bucket[iBucket]].symbol)
			{
				iUnit = UnitSymbols_HashTable[hashSymbol].bucket[iBucket];
				foundUnitLen = (int)strlen(KnownUnits[iUnit].symbol);
				break;
			}
		}
		if (iUnit != -1) { break; }
		hashLongName = hashRaw % hashTableSize_UnitLongNames;
		for (iBucket = 0; iBucket < UnitLongNames_HashTable[hashLongName].bucketSize; iBucket++)
		{
			if (tryUnitNames[iTryUnitName] == KnownUnits[UnitLongNames_HashTable[hashLongName].bucket[iBucket]].longName)
			{
				iUnit = UnitLongNames_HashTable[hashLongName].bucket[iBucket];
				foundUnitLen = (int)strlen(KnownUnits[iUnit].longName);
				break;
			}
		}
		if (iUnit != -1) { break; }
		iTryUnitName++;
	}

	//if (iUnit == -1)
	//{
	//	// try long name
	//	hash = hasher(possibleUnitPart) % hashTableSize_UnitLongNames;
	//	for (iBucket = 0; iBucket < UnitLongNames_HashTable[hash].bucketSize; iBucket++)
	//	{
	//		//if (!strcmp(possibleUnitPart, KnownUnits[UnitLongNames_HashTable[hash].bucket[iBucket]].longName))
	//		if (possibleUnitPart == KnownUnits[UnitLongNames_HashTable[hash].bucket[iBucket]].longName)
	//		{
	//			iUnit = UnitLongNames_HashTable[hash].bucket[iBucket];
	//			foundUnitLen = (int)strlen(KnownUnits[iUnit].longName);
	//			break;
	//		}
	//	}
	//
	//}

#else

	csubstr ss(nullptr, 0, 0);
	if (iUnit == -1 || (foundUnitLen < nlen))
	{
		// Nothing found so far, revert to slow linear search
		for (int i = 0; i < KnownPrefixesLength; i++)
		{
			foundPrefixLen = (int)strlen(KnownPrefixes[i].symbol);
			ss = csubstr(KnownPrefixes[i].symbol, 0, foundPrefixLen);
			if (ss.begins(name))
			{
				iPrefix = i;
				break;
			}
			foundPrefixLen = (int)strlen(KnownPrefixes[i].longName);
			ss = csubstr(KnownPrefixes[i].longName, 0, foundPrefixLen);
			if (ss.begins(name))
			{
				iPrefix = i;
				break;
			}
			foundPrefixLen = 0;
		}
		if (iPrefix == -1) { return false; }
		if (foundPrefixLen + foundUnitLen != nlen)
		{
			for (int i = 0; i < KnownUnitsLength; i++)
			{
				foundUnitLen = (int)strlen(KnownUnits[i].symbol);
				ss = csubstr(KnownUnits[i].symbol, 0, foundUnitLen);
				if (ss.ends(name))
				{
					iUnit = i;
					break;
				}
				foundUnitLen = (int)strlen(KnownUnits[i].longName);
				ss = csubstr(KnownUnits[i].longName, 0, foundUnitLen);
				if (ss.ends(name))
				{
					iUnit = i;
					break;
				}
				foundUnitLen = 0;
			}
		}
	}
#endif  // !NO_HASHING

	if (iUnit == -1) { return false; }
	if (foundUnitLen == nlen)
	{
		outPrefixIndex = -1;
		outUnitIndex = iUnit;
		return true;
	}
	else if (foundPrefixLen + foundUnitLen == nlen)
	{
		outPrefixIndex = iPrefix;
		outUnitIndex = iUnit;
		return true;
	}
	return false;
}


/**********************
signed int PhysicalQuantity::findUnit(const string& name)
{
	// TODO: convert away from string
	signed int ret;
#if defined(NO_NEW) || defined(NO_THROW)
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
**********************************************************************************/


bool PhysicalQuantity::operator==(const PhysicalQuantity& rhs) const
{
	if (memcmp(dim, rhs.dim, sizeof(dim)) != 0) { return false; }
	// TODO: use feq / tolerance comparison
	//return (value == rhs.value);
	return PhysicalQuantity::feq(value, rhs.value, equalityToleranceFactor);
}


//============================================================================================
// Display and conversion stuff

void PhysicalQuantity::PreferredUnitsBase::build(const CSubString& unitList, int* buffer, int bufferLen, bool dynamic)   //(const CSubString& unitList, int* buffer, int bufferLen, bool dynamic)
{
	//const char* pstr = unitList.c_str();
	int len = (int)unitList.length();
	int pos = 0;
	unitIndeces = buffer; //new int[4]; //staticStorage;
	int storageCapacity = bufferLen; // staticStorageSize;
	count_ = 0;
	std::pair<int, int> a;
	int wordStart = 0;
	int wordEnd = 0;
	//string word;
	csubstr word;
	while (unitList[wordStart] == ' ') { wordStart++; }
	//while ( < len && pstr[wordStart] != 0)
	char c;
	int unitIndex, prefixIndex;
	for (pos = wordStart; pos <= len; pos++)
	{
		if (pos < len) { c = unitList[pos]; }
		if (pos >= len || c == ' ')
		{
			wordEnd = pos;
			word = csubstr(unitList, wordStart, wordEnd - wordStart); //unitList.substr(wordStart, wordEnd - wordStart);

			//unitIndex = PhysicalQuantity::findUnit(word);
			//if (unitIndex != -1)
			if (PhysicalQuantity::findUnit(word, unitIndex, prefixIndex))
			{
				//prefs.push_back(unitIndex); 
				if (count_ >= storageCapacity)
				{
#ifdef NO_NEW
					// We're out of space, but ignore and finish
					return;
#else
					if (dynamic)
					{
						int newStorageCapacity = int(storageCapacity * 1.5) + 1;
						int* tmpArray = new int[newStorageCapacity];
						memcpy(tmpArray, unitIndeces, sizeof(int) * count_);
						unitIndeces = tmpArray;
						storageCapacity = newStorageCapacity;
					}
					else { return; }
#endif
				}
				unitIndeces[count_] = unitIndex;
				count_++;
			}

			wordStart = wordEnd + 1;
			while (unitList[wordStart] == ' ') { wordStart++; }
			pos = wordStart;
		}
	}
}

#ifndef NO_NEW

PhysicalQuantity::PreferredUnits_dynamic::PreferredUnits_dynamic(const CSubString& unitList)  //const string& unitList)
{
	build(unitList, nullptr, 0, true);
}
PhysicalQuantity::PreferredUnits_dynamic::~PreferredUnits_dynamic()
{
	//if (unitIndeces != staticStorage) { delete [] unitIndeces; }
	if (unitIndeces) { delete [] unitIndeces; }
}

#else // !NO_NEW

PhysicalQuantity::PreferredUnits_static::PreferredUnits_static(const CSubString& unitList, int* storage, size_t storageSizeBytes)
{
	count_ = 0;
	unitIndeces = storage;
	int capacity = (int)(storageSizeBytes / sizeof(int));
	build(unitList, storage, capacity, false);
}

#endif  // !NO_NEW

const PhysicalQuantity::UnitDefinition& PhysicalQuantity::PreferredUnitsBase::operator[] (int i)
{
	return KnownUnits[unitIndeces[i]];
}

/********************************************************************************
bool PhysicalQuantity::cstrLess::operator()(const char* a, const char* b) const
{
	return (strcmp(a, b) < 0);
}

bool PhysicalQuantity::cstrEq::operator()(const char* a, const char* b) const
{
	return (strcmp(a, b) == 0);
}
*********************************************************************************/


bool PhysicalQuantity::feq(PhysicalQuantity::num a, PhysicalQuantity::num b, PhysicalQuantity::num toleranceFactor)
{
	num margin = (a >= 0.0 ? a : -a) * toleranceFactor;
	num marginB = (b >= 0 ? b : -b) * toleranceFactor;
	if (margin < 0.0) { margin *= -1.0; }
	if (marginB < 0.0) { marginB *= -1.0; }
	if (marginB < margin) { margin = marginB; }
	num diff = b - a;
	if (diff < 0.0) { diff *= -1.0; }
	if (diff < margin) { return true; }
	else { return false; }
}


#ifdef NO_INLINE
#ifdef NO_THROW
void PhysicalQuantity::SetErrorHandler(void (*errorHandler_arg)(void* context, ErrorCode e)) { errorHandler = errorHandler_arg; }
#endif //#ifdef NO_THROW

int PhysicalQuantity::PreferredUnitsBase::count() { return count_; }
size_t PhysicalQuantity::cstrHasherTiny::operator()(const char* s) const { return operator()(CSubString(s)); }

void PhysicalQuantity::parseUnits(const char* unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut) { return parseUnits(CSubString(unitStr), unitsOut, factorOut, offsetOut); }
PhysicalQuantity::num PhysicalQuantity::convert(const char* units) const { return convert(csubstr(units)); }
bool PhysicalQuantity::findUnit(const char* pcharName, int& outUnitIndex, int& outPrefixIndex) { return findUnit(CSubString(pcharName), outUnitIndex, outPrefixIndex); }
bool PhysicalQuantity::sameUnitAs(const PhysicalQuantity& rhs) const { return isLikeQuantity(rhs); }
bool PhysicalQuantity::unitsMatch(const PhysicalQuantity& rhs) const { return isLikeQuantity(rhs); }

#endif //#ifdef NO_INLINE



