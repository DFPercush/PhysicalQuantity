/*
TODO: 
	. convert()
	. toString(const PreferredUnits& pu)
	. initializer_list ? {1.23, {0,1,0,0,0}}
	. constexpr
	. print a double without sprintf ? Arduino for example can't sprintf floating points
	.	> -48+3
		-483
*/

#include <PhysicalQuantity.h>

#ifndef NO_HASHING
#ifndef PQ_GENERATING_HASH_TABLES
#include <PhysicalQuantity/hashTables.h>
#else
struct { int bucketSize; int bucket[1]; } UnitSymbols_HashTable[1];
struct { int bucketSize; int bucket[1]; } UnitLongNames_HashTable[1];
struct { int bucketSize; int bucket[1]; } PrefixSymbols_HashTable[1];
struct { int bucketSize; int bucket[1]; } PrefixLongNames_HashTable[1];
#endif //#ifndef PQ_GENERATING_HASH_TABLES
#endif //#ifndef NO_HASHING

using namespace std;

typedef PhysicalQuantity::CSubString csubstr;

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



typedef PhysicalQuantity PQ;

void PhysicalQuantity::init()
{
	if (!(PQHeaderOptionsMatch))
	{
		throw HeaderConfigException("PhysicalQuantity: Module/object was compiled with different header options.");
	}
	value = 0.0;
	memset(dim, 0, sizeof(dim));
}

PhysicalQuantity::PhysicalQuantity()
{
	init();
}


PhysicalQuantity::PhysicalQuantity(const PhysicalQuantity& copy)
{
	init();
	value = copy.value;
	memcpy(dim, copy.dim, sizeof(dim));
}

PhysicalQuantity::PhysicalQuantity(PhysicalQuantity&& move) noexcept
{
	init();
	value = move.value;
	memcpy(dim, move.dim, sizeof(dim));
}

PhysicalQuantity::PhysicalQuantity(num value_p, const char* units_p)
{
	init();
	value = value_p;
	num unitFactor;
	num unitOfs;
	parseUnits(units_p, dim, unitFactor, unitOfs);
	value *= unitFactor;
	value += unitOfs;
}


PhysicalQuantity::PhysicalQuantity(CSubString str)
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

unsigned int PhysicalQuantity::magdim() const
{
	int ret = 0;
	int di;
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		di = dim[i];
		if (di >= 0) { ret += di; }
		else { ret -= di; }
	}
	return ret;
}

//void PhysicalQuantity::magdimMulDiv(const UnitDefinition& unit, int& mulOut, int& divOut int& powOut) const
int PhysicalQuantity::magdimReduce(const UnitDefinition& unit) const
//magdimMulDiv(const PhysicalQuantity::UnitDefinition& unit) const
{
	//int ret = 0;
	//int rmul = 0;
	////int rdiv = 0;
	//int test;
	
	//int maxpow = 0;
	//int maxFactor = 1;

	unsigned int mdorig = magdim();
	int after = 0;
	for (int iq = 0; iq < (int)QuantityType::ENUM_MAX; iq++)
	{
		after += dim[iq] - unit.dim[iq];
	}
	int mdUnitDiff = ((after >= 0) ? after : -after) - (int)mdorig;

	if (mdUnitDiff == 0) 
	{
		return 0;
	}
	int direction = (mdUnitDiff < 0) ? 1 : -1;
	int testpow = 0;
	int lastpow;
	unsigned int mdlast;
	after = mdorig;
	do
	{
		mdlast = after;
		lastpow = testpow;
		testpow += direction;
		after = 0;
		for (int iq = 0; iq < (int)QuantityType::ENUM_MAX; iq++)
		{
			after += abs(dim[iq] - (unit.dim[iq] * testpow));
		}
		if (after < 0) { after = -after; }
	} while (after < (int)mdlast);
	return lastpow;

	//do
	//{
	//	testpow++;
	//	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	//	{
	//		//
	//
	//		//test = dim[i] - unit.dim[i] * testpow;
	//		//if (test >= 0) { rdiv += test; }
	//		//else { rdiv -= test; }
	//		//
	//		//test = dim[i] + unit.dim[i];
	//		//if (test >= 0) { rmul += test; }
	//		//else { rmul -= test; }
	//
	//		//di = dim[i] - unit.dim[i];
	//		//if (di >= 0) { ret += di; }
	//		//else { ret -= di; }
	//	}
	//} while (maxFactor > 0);
	//mulOut = rmul;
	//divOut = rdiv;
}


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
		else { c = 0; }
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
					ssUnitName = word.substr(0, iCaret);
					powerStr = word.substr(iCaret + 1);
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

				if (ssUnitName.length() > 0)
				{
					if (!findUnit(ssUnitName, foundUnit, foundPrefix))
					{
	#ifdef NO_THROW
						errorHandler(errorUserContext, E_INVALID_EXPRESSION);
						return;
	#else
						throw InvalidExpressionException("Unit not found");
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
				}  //if (ssUnitName.length() > 0)
			} // end 'else' meaning not a key word / char

			// Advance to next word
			wordStart = wordEnd;
			while (unitStr[wordStart] == ' ' && wordStart < ulen) { wordStart++; }
			i = wordStart;
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
	memcpy(unitsOut_arg, unitsOut, sizeof(unitsOut));
}


void PhysicalQuantity::mulUnit(signed char(&unitsOut)[(int)QuantityType::ENUM_MAX], const UnitDefinition& unit, signed int power, bool invert)
{
	if (invert) { power *= -1; }
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		unitsOut[i] += power * unit.dim[i];
	}
}

void PhysicalQuantity::parse(const CSubString& text_arg)
{
	CSubString text(text_arg);
	num newValue = 0.0;
	int firstNotSpace = (int)text.find_first_not_of(' ');
	if (firstNotSpace > 0) { text = text.substr(firstNotSpace); }
	if (text.length() == 0)
	{
#ifdef NO_THROW
		errorHandler(errorUserContext, E_INVALID_EXPRESSION);
		return;
#else
		throw InvalidExpressionException("Quantity can not be null."); 
#endif
	}
	//int firstNotNumber = (int)text.find_first_not_of("0123456789Ee.-+"); // added Ee
	//if (firstNotNumber == -1)
	//{
	//	value = text.atof();
	//	memset(dim, 0, sizeof(dim));
	//	return;
	//}
	//else
	//{
	//}
	
	int firstSpace = (int)text.find_first_of(' ');
	csubstr unitStr;
	if (text.substr(0, firstSpace).isnumber())
	{
		value = text.atof();
		if (firstSpace == -1)
		{
			unitStr = "";
		}
		else
		{
			unitStr = text.substr(firstSpace + 1);
		}
	}
	else
	{
		value = 1.0;
		unitStr = text;
	}
	num unitFactor = 1.0;
	num unitOfs = 0.0;
	//parseUnits(CSubString(text, firstNotNumber), dim, unitFactor, unitOfs);
	parseUnits(unitStr, dim, unitFactor, unitOfs);
	value *= unitFactor;
	value += unitOfs;
}

void PhysicalQuantity::WriteOutputUnit(int plen, int ulen, int reduceExp, int &outofs, int size, char * buf, int ipre, const PhysicalQuantity::UnitDefinition & testunit) const
{
	int reduceExpLen = 0;
	int nextLengthNeeded = plen + ulen + 1;
	if (reduceExp > 1) 
	{
		reduceExpLen = (int)log10(reduceExp) + 1;
		nextLengthNeeded += reduceExpLen; 
	}
	else if (reduceExp < 0) // -1) 
	{
		reduceExpLen = (int)log10(-reduceExp) + 2;
		nextLengthNeeded += reduceExpLen; 
	}
	if (outofs + nextLengthNeeded < size)
	{
		// write space
		buf[outofs++] = ' ';
		// write prefix
		if (plen > 0 && ipre != -1) 
		{
			memcpy(buf + outofs, KnownPrefixes[ipre].symbol, plen); 
			outofs += plen;
		}
		// write unit
		memcpy(buf + outofs, testunit.symbol, ulen);
		outofs += ulen;
		// write power
		if (reduceExp != 1)
		{
			buf[outofs++] = '^';
			sprintf(buf + outofs, "%d", reduceExp);
			outofs += reduceExpLen;
		}
	}
	else
	{
		outofs += nextLengthNeeded; //plen + ulen + 1;
	}
}
void PhysicalQuantity::sprintHalf(PhysicalQuantity& r, const PhysicalQuantity::PreferredUnitsBase & pu, bool& hasDenom, bool inDenomNow, int &md, int origmd, bool useSlash, int &outofs, int size, char * buf) const
{
	//int ulen;
	//int plen;
	//int ipre;
	//int reduceExp;
	//bool goForOutput;
	hasDenom = false;
	for (int ipu = 0; ipu < pu.count() && md != 0; ipu++)
	{
		const UnitDefinition& testunit = KnownUnits[pu[ipu].iUnit];
		int ipre = pu[ipu].iPrefix;
		int plen;
		if (ipre == -1) { plen = 0; }
		else { plen = (int)strlen(KnownPrefixes[ipre].symbol); }
		sprintHalfTryUnit(testunit, r, origmd, hasDenom, useSlash, inDenomNow, plen, outofs, size, buf, ipre, md);
	}

	// That's all the PreferredUnits. Is there anything left over?
	for (int iu = 0; iu < KnownUnitsLength && md != 0; iu++)
	{
		const UnitDefinition& testunit = KnownUnits[iu];
		sprintHalfTryUnit(testunit, r, origmd, hasDenom, useSlash, inDenomNow, 0, outofs, size, buf, -1, md);
	}
}
void PhysicalQuantity::sprintHalfTryUnit(const PhysicalQuantity::UnitDefinition & testunit, PhysicalQuantity & r, int origmd, bool & hasDenom, bool useSlash, bool inDenomNow, int plen, int & outofs, int size, char * buf, int ipre, int & md) const
{
	int ulen = (int)strlen(testunit.symbol);
	int reduceExp = r.magdimReduce(testunit);
	if (reduceExp != 0)
	{
		// if it reduces the overall dimension of the value, use it.
		if (origmd == 1 && testunit.offset != 0.0)
		{
			r.value -= testunit.offset;
		}
		r.value /= testunit.factor;
		mulUnit(r.dim, testunit, reduceExp, true);
		if (ipre != -1) { r.value /= KnownPrefixes[ipre].factor; }
		bool goForOutput = false;
		if (reduceExp < 0)
		{
			hasDenom = true;
			if (!useSlash || inDenomNow)
			{
				goForOutput = true;
			}
			if (inDenomNow) reduceExp *= -1;
		}
		else { goForOutput = !inDenomNow; } // if (!inDenomNow) { }
		if (goForOutput) 
		{
			WriteOutputUnit(plen, ulen, reduceExp, outofs, size, buf, ipre, testunit); 
		}
	}
	md = r.magdim();
}
size_t PhysicalQuantity::sprint(char* buf, int size, const PreferredUnitsBase& pu, bool useSlash) const
{
	PhysicalQuantity r = *this;
	int md, origmd;
	origmd = magdim();
	md = origmd;
	int outofs = 0;
	bool hasDenom = false;

	sprintHalf(r, pu, hasDenom, false, md, origmd, useSlash, outofs, size, buf);
	if (hasDenom && useSlash)
	{
		buf[outofs++] = ' ';
		buf[outofs++] = '/';
		md = origmd;
		r = *this;
		sprintHalf(r, pu, hasDenom, true, md, origmd, useSlash, outofs, size, buf);
	}

	if (md != 0)
	{
#ifdef NO_THROW
		errorHandler(errorUserContext, E_LOGIC_ERROR);
		return outOfs + 1;
#else
		throw logic_error("units left over after conversion");
#endif
	}

	// TODO: Some systems do not support sprintf("%g...");
	if (outofs + MAX_NUM_TEXT_LENGTH > size) { return outofs + MAX_NUM_TEXT_LENGTH; }
	sprintf(buf + outofs, "%g", r.value);
	int numofs = outofs;
	int numlen = (int)strlen(buf + numofs);
	outofs += numlen;
	// Now shuffle the number to the beginning
	// TODO: Possible speed improvement if we have some extra padding at the end of the buffer
	char c;
	for (int isubpos = 0; isubpos < numlen; isubpos++)
	{
		c = buf[numofs + isubpos];
		for (int iBubble = numofs + isubpos; iBubble > isubpos; iBubble--)
		{
			buf[iBubble] = buf[iBubble - 1];
		}
		buf[isubpos] = c;
	}

	if (outofs < size) { buf[outofs] = 0; }
	else if (size > 0) { buf[size - 1] = 0; }
	return outofs + 1;
}

#if !defined(NO_STD_STRING) && !defined(NO_PRINTF)

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

#endif //#if !defined(NO_STD_STRING) && !defined(NO_PRINTF)

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
	int nlen = name.length();
	csubstr possibleUnitPart;
	int foundPrefixLen = 0;
	int foundUnitLen = 0;

#ifndef NO_HASHING
	static cstrHasherTiny hasher;
	unsigned int iBucket, hashRaw, hashSymbol, hashLongName;
	char firstLetter[2];

	if ((nlen > 2) && (name[0] == 'd') && (name[1] == 'a'))
	{
		possibleUnitPart = csubstr(name, 2);
		hashRaw = (unsigned int)hasher(possibleUnitPart);
		hashSymbol = hashRaw % hashTableSize_UnitSymbols;
		for (iBucket = 0; iBucket < UnitSymbols_HashTable[hashSymbol].bucketSize; iBucket++)
		{
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
	hashSymbol = (unsigned int)hasher(firstLetter) % hashTableSize_PrefixSymbols;
	for (iBucket = 0; iBucket < PrefixSymbols_HashTable[hashSymbol].bucketSize; iBucket++)
	{
		if (name[0] == KnownPrefixes[PrefixSymbols_HashTable[hashSymbol].bucket[iBucket]].symbol[0])
		{
			iPrefix = PrefixSymbols_HashTable[hashSymbol].bucket[iBucket];
			foundPrefixLen = (int)strlen(KnownPrefixes[iPrefix].symbol);
			break;
		}
	}

	csubstr tryUnitNames[2];
	tryUnitNames[0] = csubstr(name);
	tryUnitNames[1] = csubstr(name, 1);
	int iTryUnitName = 0; // (iPrefix == -1) ? 1 : 0;

	// look up unit symbol
	while(iTryUnitName < 2)
	{
		hashRaw = (unsigned int)hasher(tryUnitNames[iTryUnitName]);
		hashSymbol = hashRaw % hashTableSize_UnitSymbols;
		for (iBucket = 0; iBucket < UnitSymbols_HashTable[hashSymbol].bucketSize; iBucket++)
		{
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
	return PhysicalQuantity::feq(value, rhs.value, equalityToleranceFactor);
}


//============================================================================================
// Display and conversion stuff

void PhysicalQuantity::PreferredUnitsBase::build(const CSubString& unitList, PhysicalQuantity::PreferredUnitsBase::UnitPref* buffer, int bufferSizeBytes, bool dynamic)   //(const CSubString& unitList, int* buffer, int bufferLen, bool dynamic)
{
	//const char* pstr = unitList.c_str();
	int len = (int)unitList.length();
	int pos = 0;
	unitIndeces = buffer; //new int[4]; //staticStorage;
	int bufferLen = bufferSizeBytes / sizeof(UnitPref);
	int storageCapacity = bufferLen; // staticStorageSize;
	count_ = 0;
	//std::pair<int, int> a; // TODO: remove
	int wordStart = 0;
	int wordEnd = 0;
	csubstr word;
	while (unitList[wordStart] == ' ') { wordStart++; }
	char c;
	int unitIndex, prefixIndex;
	int iCaret;
	for (pos = wordStart; pos <= len; pos++)
	{
		if (pos < len) { c = unitList[pos]; }
		if (pos >= len || c == ' ')
		{
			wordEnd = pos;
			word = csubstr(unitList, wordStart, wordEnd - wordStart); //unitList.substr(wordStart, wordEnd - wordStart);
			iCaret = word.find_first_of('^');
			if (iCaret != -1) { word = word.substr(0, iCaret); }

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
						//int* tmpArray = new int[newStorageCapacity];
						UnitPref* tmpArray = new UnitPref[newStorageCapacity];
						if (unitIndeces)
						{
							memcpy(tmpArray, unitIndeces, sizeof(UnitPref) * count_);
							delete [] unitIndeces;
						}
						unitIndeces = tmpArray;
						storageCapacity = newStorageCapacity;
					}
					else { return; }
#endif
				}
				unitIndeces[count_].iUnit = unitIndex;
				unitIndeces[count_].iPrefix = prefixIndex;
				count_++;
			}

			wordStart = wordEnd + 1;
			while (unitList[wordStart] == ' ') { wordStart++; }
			pos = wordStart;
		}
	}
}

PhysicalQuantity::PreferredUnitsBase::~PreferredUnitsBase()
{
}

#ifndef NO_NEW

PhysicalQuantity::PreferredUnits_dynamic::PreferredUnits_dynamic(const CSubString& unitList)  //const string& unitList)
{
	unitIndeces = nullptr;
	build(unitList, nullptr, 0, true);
}
PhysicalQuantity::PreferredUnits_dynamic::~PreferredUnits_dynamic()
{
	//if (unitIndeces != staticStorage) { delete [] unitIndeces; }
	if (unitIndeces) 
	{
		delete [] unitIndeces; 
	}
}

#else // !NO_NEW

PhysicalQuantity::PreferredUnits_static::PreferredUnits_static(const CSubString& unitList, void* storage, size_t storageSizeBytes)
{
	count_ = 0;
	unitIndeces = storage;
	int capacity = (int)(storageSizeBytes / sizeof(int));
	build(unitList, storage, capacity, false);
}

#endif  // !NO_NEW

const PhysicalQuantity::PreferredUnitsBase::UnitPref& PhysicalQuantity::PreferredUnitsBase::operator[] (int i) const
{
	return unitIndeces[i]; // KnownUnits[unitIndeces[i]];
}

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

PhysicalQuantity PhysicalQuantity::eval(CSubString str)
{
	// trying to avoid any dynamic allocation here,
	// even if it takes a few extra passes
	//int pstart;
	int pleft = -1;
	int pright = -1;
	int plevel = 0;
	PQ left;
	PQ right;
	//PQ val;
	char prev = 0;
	char c;
	int len = str.length();
	//for (int i = 0; i < len; i++)
	plevel = 0;
	for (int i = len - 1; i >= 0; i--)
	{
		c = str[i];
		if (c == ')')
		{
			if (plevel == 0) { pright = i; }
			plevel++;
		}
		else if (c == '(')
		{
			if (plevel == 0)
			{
#ifdef NO_THROW
				errorHandler(errorUserContext, E_INVALID_EXPRESSION);
				return PQ("0");
#else
				throw InvalidExpressionException("Closed parentheses without matching open");
#endif
			}
			pleft = i;
			plevel--;
		}
		else if (c == '+' && plevel == 0 && str[i+1] == ' ')
		{
			left = eval(str.substr(0, i - 1));
			right = eval(str.substr(i + 1));
			return left + right;
		}
		else if (c == '-' && plevel == 0 && str[i+1] == ' ')
		{
			left = eval(str.substr(0, i - 1));
			right = eval(str.substr(i + 1));
			return left - right;
		}
		prev = c;
	}

	plevel = 0;
	for (int i = len - 1; i >= 0; i--)
	{
		c = str[i];
		if (c == ')')
		{
			if (plevel == 0) { pright = i; }
			plevel++;
		}
		else if (c == '(')
		{
			if (plevel == 0)
			{
#ifdef NO_THROW
				errorHandler(errorUserContext, E_INVALID_EXPRESSION);
				return PQ("0");
#else
				throw InvalidExpressionException("Closed parentheses without matching open");
#endif
			}
			pleft = i;
			plevel--;
		}
		else if (c == '*' && plevel == 0)
		{
			left = eval(str.substr(0, i - 1));
			right = eval(str.substr(i + 1));
			return left * right;
		}
		else if (c == '/' && plevel == 0)
		{
			left = eval(str.substr(0, i));
			right = eval(str.substr(i + 1));
			return left / right;
		}
		prev = c;
	}

	plevel = 0;
	for (int i = len - 1; i >= 0; i--)
	{
		c = str[i];
		if (c == ')')
		{
			if (plevel == 0) { pright = i; }
			plevel++;
		}
		else if (c == '(')
		{
			if (plevel == 0)
			{
#ifdef NO_THROW
				errorHandler(errorUserContext, E_INVALID_EXPRESSION);
				return PQ("0");
#else
				throw InvalidExpressionException("Closed parentheses without matching open");
#endif
			}
			pleft = i;
			plevel--;
		}
		// Exponent operator ^ is parsed if previous character is not a letter,
		// in which case it's considered part of a unit
		else if (c == '^' && plevel == 0 &&
			(!(str[i - 1] >= 'a' && str[i - 1] <= 'z')) &&
			(!(str[i - 1] >= 'A' && str[i - 1] <= 'Z'))
			)
		{
			PQ mant = eval(str.substr(0, i - 1));
			PQ exp(str.substr(i + 1));
			for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
			{
				if (exp.dim[i] != 0)
				{
#ifdef NO_THROW
					errorHandler(errorUserContext, E_INVALID_EXPRESSION);
					return val;
#else
					throw InvalidExpressionException("Exponent must be a scalar");
#endif
				}
			}
			if (exp.value != (num)((int)exp.value))
			{
				for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
				{
					if (mant.dim[i] != 0)
					{
#ifdef NO_THROW
						errorHandler(errorUserContext, E_INVALID_EXPRESSION);
						return PQ("0");
#else
						throw InvalidExpressionException("Exponent of a quantity with units must be an integer");
#endif
					}
				}
			}
			mant.value = pow(mant.value, exp.value);
			for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
			{
				int newpow = mant.dim[i] * (int)exp.value;
				//overflow_error
				if (newpow > 127 || newpow < -128)
				{
#ifdef NO_THROW
					errorHandler(errorUserContext, E_OVERFLOW);
					return PQ("0");
#else
					throw overflow_error("Unit exponent too large");
#endif
				}
			}
			return mant;
		}
		prev = c;
	}


	if (plevel != 0 ||
		(pleft == -1 && pright != -1) ||
		(pleft != -1 && pright == -1))
	{
#ifdef NO_THROW
		errorHandler(errorUserContext, E_INVALID_EXPRESSION);
		return val;
#else
		throw InvalidExpressionException("Mismatched parentheses");
#endif
	}


	if (pleft == -1)
	{
		return PQ(str);
	}
	else 
	{
		left = eval(str.substr(pleft + 1, pright - pleft - 1));
		return left;
	}
}

size_t PhysicalQuantity::sprint(char* buf, int size, const csubstr& sspu, bool useSlash) const
{
	return sprint(buf, size, PreferredUnits(sspu), useSlash);
}


#ifdef NO_INLINE
#ifdef NO_THROW
void PhysicalQuantity::SetErrorHandler(void (*errorHandler_arg)(void* context, ErrorCode e)) { errorHandler = errorHandler_arg; }
#endif //#ifdef NO_THROW

int PhysicalQuantity::PreferredUnitsBase::count() const { return count_; }
size_t PhysicalQuantity::cstrHasherTiny::operator()(const char* s) const { return operator()(CSubString(s)); }

PhysicalQuantity::PhysicalQuantity(const char* str) { PhysicalQuantity(CSubString(str)); }
void PhysicalQuantity::parseUnits(const char* unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut) { return parseUnits(CSubString(unitStr), unitsOut, factorOut, offsetOut); }
PhysicalQuantity::num PhysicalQuantity::convert(const char* units) const { return convert(csubstr(units)); }
bool PhysicalQuantity::findUnit(const char* pcharName, int& outUnitIndex, int& outPrefixIndex) { return findUnit(CSubString(pcharName), outUnitIndex, outPrefixIndex); }
bool PhysicalQuantity::sameUnitAs(const PhysicalQuantity& rhs) const { return isLikeQuantity(rhs); }
bool PhysicalQuantity::unitsMatch(const PhysicalQuantity& rhs) const { return isLikeQuantity(rhs); }
void PhysicalQuantity::parse(const char* text) { parse(csubstr(text)); }
size_t PhysicalQuantity::sprint(char* buf, int size, const char* pu, bool useSlash) const { return sprint(buf, size, PreferredUnits(pu), useSlash); }
size_t PhysicalQuantity::sprint(char* buf, int size, bool useSlash) const {	return sprint(buf, size, PreferredUnits(""), useSlash); }
#endif //#ifdef NO_INLINE


#ifndef NO_LITERALS
// TODO: Also define with all prefixes?
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
DefineLiteral(rad)
DefineLiteral(deg)
DefineLiteral(lb)

#endif  // !NO_LITERALS


