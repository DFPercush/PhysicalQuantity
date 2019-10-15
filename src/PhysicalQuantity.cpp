/*
TODO: 
	. initializer_list ? {1.23, {0,1,0,0,0}}
*/

#include <PhysicalQuantity.h>
#include <math.h>
//#include <memory.h>
#include <string.h>
#include <stdio.h>

#ifndef NO_HASHING
#ifndef PQ_GENCODE
#include <PhysicalQuantity/hashTables.ah>
#endif //#ifndef PQ_GENCODE
#endif //#ifndef NO_HASHING

using namespace std;

#ifndef NO_TEXT
typedef PhysicalQuantity::CSubString csubstr;
#endif

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#ifdef PQ_DEBUG_EVAL
#define DEBUG_LR_STR(leftstr, rightstr, op) printf("  %s : \"%s\"  <<< %s >>>  \"%s\"\n", op, leftstr.toStdString().c_str(), op, rightstr.toStdString().c_str())
#define DEBUG_LR_RESULT(left, right, op, result) printf("    #: %s  <<< %s >>>  %s ==> %s\n", left.toString().c_str(), op, right.toString().c_str(), result.toString().c_str())
#else
#define DEBUG_LR_STR(leftstr, rightstr, op)
#define DEBUG_LR_RESULT(left, right, op, result)
#endif


PhysicalQuantity::num PhysicalQuantity::equalityToleranceFactor = 1e-6;
const int PhysicalQuantity::compiledHeaderOptions = PQ_HEADER_OPTIONS;

#ifdef NO_THROW
void (*PhysicalQuantity::errorHandler)(void* userContext, ErrorCode e);
void* PhysicalQuantity::errorUserContext;
#else
PhysicalQuantity::UnitMismatchException::UnitMismatchException() : std::runtime_error("") {}
PhysicalQuantity::UnitMismatchException::UnitMismatchException(const char* message) : std::runtime_error(message) {}
PhysicalQuantity::InvalidExpressionException::InvalidExpressionException() : std::runtime_error("") {}
PhysicalQuantity::InvalidExpressionException::InvalidExpressionException(const char* message) : std::runtime_error(message) {}
PhysicalQuantity::HeaderConfigException::HeaderConfigException() : std::runtime_error("") {}
PhysicalQuantity::HeaderConfigException::HeaderConfigException(const char* message) : std::runtime_error(message) {}

#endif



typedef PhysicalQuantity PQ;


#ifdef ROM_READ_BYTE
#define IFROM(normal_expr, rom_expr) rom_expr
void romcpy(void* to, const void* from, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		*(((char*)to) + i) = ROM_READ_BYTE((char*)from + i);
	}
}
size_t romstrlen(const char* s)
{
	size_t ret = 0;
	while (ROM_READ_BYTE(s + ret)) { ret++; }
	return ret;
}
int memcmp_romleft(const char* left, const char* right, size_t len)
{
	char cleft;
	char cright;
	for (size_t i = 0; i < len; i++)
	{
		cleft = ROM_READ_BYTE(left + i);
		cright = right[i];
		if (cleft < cright) { return -1; }
		if (cleft > cright) { return 1; }
	}
	return 0;
}
int memcmp_romright(const char* left, const char* right, size_t len)
{
	char cleft;
	char cright;
	for (size_t i = 0; i < len; i++)
	{
		cleft = left[i];
		cright = ROM_READ_BYTE(right + i);
		if (cleft < cright) { return -1; }
		if (cleft > cright) { return 1; }
	}
	return 0;
}
int memcmp_romboth(const char* left, const char* right, size_t len)
{
	char cleft;
	char cright;
	for (size_t i = 0; i < len; i++)
	{
		cleft = ROM_READ_BYTE(left + i);
		cright = ROM_READ_BYTE(right + i);
		if (cleft < cright) { return -1; }
		if (cleft > cright) { return 1; }
	}
	return 0;
}

#else
#define IFROM(normal_expr, rom_expr) normal_expr
#endif

bool InitLibPQ(int headerOptionFlags)
{
	if (headerOptionFlags != PQ_HEADER_OPTIONS)
	{
		return false;
	}
	return true;
}

void PhysicalQuantity::init()
{
//	if (!(PQHeaderOptionsMatch))
//	{
//#ifdef NO_THROW
//		errorHandler(errorUserContext, E_HEADER_CONFIG);
//#else
//		throw HeaderConfigException("PhysicalQuantity: Module/object was compiled with different header options.");
//#endif
//	}
	value = 0.0;
	memset(dim, 0, sizeof(dim));
}

PhysicalQuantity::PhysicalQuantity()
{
	init();
}


PhysicalQuantity::PhysicalQuantity(const PhysicalQuantity& copy)
{
	//init();
	value = copy.value;
	memcpy(dim, copy.dim, sizeof(dim));
}

PhysicalQuantity::PhysicalQuantity(PhysicalQuantity&& move) noexcept
{
	//init();
	value = move.value;
	memcpy(dim, move.dim, sizeof(dim));
}

#ifndef NO_TEXT
PhysicalQuantity::PhysicalQuantity(num value_p, const char* units_text)
{
	init();
	value = value_p;
	num unitFactor;
	num unitOfs;
	parseUnits(units_text, dim, unitFactor, unitOfs);
	value *= unitFactor;
	value += unitOfs;
}

PhysicalQuantity::PhysicalQuantity(CSubString str)
{
	init();
	parse(str);
}
#endif //#ifndef NO_TEXT

PhysicalQuantity::PhysicalQuantity(PhysicalQuantity::num valueArg)
{
	init();
	value = valueArg;
}

#if !defined(YES_CONSTEXPR) // || defined(NO_INLINE)
PhysicalQuantity::PhysicalQuantity(num value_p, const signed char dim_p[(int)QuantityType::ENUM_MAX])
	: value(value_p), dim {dim_p[0], dim_p[1], dim_p[2], dim_p[3], dim_p[4]} {}
#endif


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

#ifndef NO_TEXT
PhysicalQuantity::num PhysicalQuantity::convert(const CSubString& units) const
{
	signed char tdim[(int)QuantityType::ENUM_MAX];
	num factor;
	num offset;
	parseUnits(units, tdim, factor, offset);
	if (memcmp(dim, tdim, sizeof(dim)) != 0)
	{
#ifdef NO_THROW
		errorHandler(errorUserContext, E_UNIT_MISMATCH);
		return 0.0;
#else
		throw UnitMismatchException("");
#endif //#ifdef NO_THROW
	}
	return (value - offset) / factor;
}
#endif //#ifndef NO_TEXT

bool PhysicalQuantity::like(const PhysicalQuantity& rhs) const
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

#ifndef NO_TEXT
int PhysicalQuantity::magdimReduce(const UnitDefinition& unit) const
{
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
	int absi;
	do
	{
		mdlast = after;
		lastpow = testpow;
		testpow += direction;
		after = 0;
		for (int iq = 0; iq < (int)QuantityType::ENUM_MAX; iq++)
		{
			absi = dim[iq] - (unit.dim[iq] * testpow);
			if (absi < 0) { absi *= -1; }
			after += absi;
		}
		if (after < 0) { after = -after; }
	} while (after < (int)mdlast);
	return lastpow;
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
	prefixIndex_t foundPrefix = -1;
	unitIndex_t foundUnit = -1;
	int iCaret;
	bool denom = false;
	csubstr ssUnitName, powerStr;
	int upow = 0;
	bool denomNext = false;
	char hasOffsetMask = 0;

#ifdef ROM_READ_BYTE
	char foundUnitVal[sizeof(UnitDefinition)]; // const array dim means we have to be roundabout
	Prefix foundPrefixVal;
#endif

	for (int i = wordStart; i <= ulen; i++)
	{
		if (denomNext) { denom = true; }
		if (i < ulen) { c = unitStr[i]; }
		else { c = 0; }
		if (c == ' ' || c == '*' || c == '/' || i == ulen) 
		{
			wordEnd = i;
			word = csubstr(unitStr, wordStart, wordEnd - wordStart);
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
				
#ifdef ROM_READ_BYTE
					romcpy(&foundUnitVal, &(KnownUnits[foundUnit]), sizeof(UnitDefinition));
					if (foundPrefix >= 0)
					{
						romcpy(&foundPrefixVal, &(KnownPrefixes[foundPrefix]), sizeof(Prefix));
					}
#endif
					if (foundPrefix >= 0)
					{
						//factorOut *= KnownPrefixes[foundPrefix].factor;
						factorOut *= ::pow(IFROM(KnownPrefixes[foundPrefix], foundPrefixVal).factor, upow);
					}
					if (foundUnit < KnownUnitOffsetsLength)
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
						
#ifdef ROM_READ_BYTE
						romcpy(&tempOfs, &(KnownUnitOffsets[foundUnit]), sizeof(KnownUnitOffsets[0]));
#else
						tempOfs = KnownUnitOffsets[foundUnit];
#endif

						for (int iSetOffsetFlags = 0; iSetOffsetFlags < (int)QuantityType::ENUM_MAX; iSetOffsetFlags++)
						{
							if (IFROM(KnownUnits[foundUnit].dim[iSetOffsetFlags],
								((UnitDefinition*)foundUnitVal)->dim[iSetOffsetFlags])
								!= 0)
							{
								hasOffsetMask |= (1 << iSetOffsetFlags);
							}
						}
					}
					factorOut *= ::pow(IFROM(KnownUnits[foundUnit].factor, ((UnitDefinition*)foundUnitVal)->factor), upow);
					mulUnit(unitsOut, IFROM(KnownUnits[foundUnit], *(UnitDefinition*)foundUnitVal), upow, denom);
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
			if ((unitsOut[iCheckDim]) != 0 && (!(hasOffsetMask & (1 << iCheckDim))))
			{
				// Units are present which are not offset
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
	CSubString text = text_arg.trim();
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
	parseUnits(unitStr, dim, unitFactor, unitOfs);
	value *= unitFactor;
	value += unitOfs;
}

void PhysicalQuantity::WriteOutputUnit(int plen, int ulen, int reduceExp, size_t &outofs, size_t size, char * buf, PhysicalQuantity::prefixIndex_t ipre, const PhysicalQuantity::UnitDefinition & testunit) const
{
	size_t reduceExpLen = 0;
	size_t nextLengthNeeded = plen + ulen + 1;
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
#ifdef ROM_READ_BYTE
			romcpy(buf + outofs, KnownPrefixes[ipre].symbol, plen);
#else
			memcpy(buf + outofs, KnownPrefixes[ipre].symbol, plen); 
#endif
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
		outofs += nextLengthNeeded;
	}
}
void PhysicalQuantity::sprintHalf(PhysicalQuantity& r, const PhysicalQuantity::UnitListBase & pu, bool& hasDenom, bool inDenomNow, int &md, int origmd, bool useSlash, size_t &outofs, size_t size, char * buf) const
{
	hasDenom = false;
	for (int ipu = 0; ipu < pu.count(); ipu++)
	{
		prefixIndex_t ipre = pu[ipu].iPrefix;
		int plen;
		if (ipre == -1) { plen = 0; }
		else { plen = (int)IFROM(strlen, romstrlen)(KnownPrefixes[ipre].symbol); }
		sprintHalfTryUnit(pu[ipu].iUnit, r, origmd, hasDenom, useSlash, inDenomNow, plen, outofs, size, buf, ipre, md, true);
	}

	// That's all the preferred units. Is there anything left over?
	if (dim[(int)QuantityType::MASS] > 0)
	{
		// Try kilogram first as a special case, because its standard unit combines a prefix.
		sprintHalfTryUnit(gramIndex, r, origmd, hasDenom, useSlash, inDenomNow, 1, outofs, size, buf, kiloIndex, md, false);
	}
 	for (unitIndex_t iu = 0; iu < KnownUnitsLength && md != 0; iu++)
	{
		sprintHalfTryUnit(iu, r, origmd, hasDenom, useSlash, inDenomNow, 0, outofs, size, buf, -1, md, false);
	}
}
void PhysicalQuantity::sprintHalfTryUnit(PQ::unitIndex_t iTestUnit, PhysicalQuantity & r, int origmd, bool & hasDenom, bool useSlash, bool inDenomNow, int plen, size_t & outofs, size_t size, char * buf, PhysicalQuantity::prefixIndex_t ipre, int & md, bool preferred) const
{
#ifdef ROM_READ_BYTE	
	char testUnitBuf[sizeof(UnitDefinition)];
	romcpy(testUnitBuf, &(KnownUnits[iTestUnit]), sizeof(UnitDefinition));
	const UnitDefinition& testunit = *(UnitDefinition*)testUnitBuf;
	Prefix preval;
	if (ipre >= 0) { romcpy(&preval, &KnownPrefixes[ipre], sizeof(Prefix)); }
#else
	const PhysicalQuantity::UnitDefinition & testunit = PQ::KnownUnits[iTestUnit];
#endif
	int ulen = (int)strlen(testunit.symbol);
	int reduceExp;
	unsigned int mdorig = magdim();
	bool noDiff = false;
	if (preferred)
	{
		PQ testPref(*this);
		mulUnit(testPref.dim, testunit, 1, false);
		if (testPref.magdim() == mdorig)
		{
			noDiff = true; 
			reduceExp = 1;
		}
		else
		{
			testPref = *this;
			mulUnit(testPref.dim, testunit, -1, false);
			if (testPref.magdim() == mdorig)
			{
				noDiff = true;
				reduceExp = -1;
			}
		}
	}

	if (!noDiff)
	{
		reduceExp = r.magdimReduce(testunit);
	}
	if (reduceExp != 0)
	{
		// if it reduces the overall dimension of the value, use it.
		if (origmd == 1 && iTestUnit < KnownUnitOffsetsLength)
		{
		#ifdef ROM_READ_BYTE
			num rrofs;
			romcpy(&rrofs, &KnownUnitOffsets[iTestUnit], sizeof(num));
			r.value -= rrofs;
		#else
			r.value -= PQ::KnownUnitOffsets[iTestUnit];
		#endif
		}
		r.value /= ::pow(testunit.factor, reduceExp);
		mulUnit(r.dim, testunit, reduceExp, true);
		if (ipre != -1) { r.value /= ::pow(IFROM(KnownPrefixes[ipre].factor, preval.factor), reduceExp); }
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
size_t PhysicalQuantity::sprint(char* buf, size_t size, unsigned int precision, const UnitListBase& pu, bool useSlash) const
{
	PhysicalQuantity r = *this;
	int md, origmd;
	origmd = magdim();
	md = origmd;
	size_t outofs = 0;
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
		return outofs + 1;
#else
		throw logic_error("units left over after conversion");
#endif
	}

	if (outofs + MAX_NUM_TEXT_LENGTH > size) { return outofs + MAX_NUM_TEXT_LENGTH; }
	//sprintf(buf + outofs, "%g", r.value);
	printNum(buf + outofs, size - outofs, r.value, precision);
	size_t numofs = outofs;
	size_t numlen = strlen(buf + numofs);
	outofs += numlen;
	// Now shuffle the number to the beginning
	// TODO: Possible speed improvement if we have some extra padding at the end of the buffer
	char c;
	for (size_t isubpos = 0; isubpos < numlen; isubpos++)
	{
		c = buf[numofs + isubpos];
		for (size_t iBubble = numofs + isubpos; iBubble > isubpos; iBubble--)
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
	//sprintf(numbuf, "%g", value);
	printNum(numbuf, 100, value, 
#ifdef LOW_PRECISION
	8);
#else
	18);
#endif
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
	if (dim[(int)QuantityType::CURRENT] > 0) { ret += " C"; }
	if (dim[(int)QuantityType::CURRENT] > 1) 
	{
		sprintf(numbuf, "%d", (static_cast<signed int>(dim[(int)QuantityType::CURRENT])));
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
	if (dim[(int)QuantityType::CURRENT] < 0) { ret += " C"; }
	if (dim[(int)QuantityType::CURRENT] < -1) 
	{
		sprintf(numbuf, "%d", (-1 * static_cast<signed int>(dim[(int)QuantityType::CURRENT])));
		ret += "^";
		ret += numbuf;
	}

	return ret;
}

string PhysicalQuantity::toString(const UnitListBase& pu) const
{
	string ret;
	size_t buflen = 0;
	size_t needbuflen = pu.count() * 12 + 32;
	char* buf = nullptr;
	while (needbuflen >= buflen)
	{
		if (buf) delete [] buf;
		buf = new char[needbuflen];
		buflen = needbuflen;
		needbuflen = sprint(buf, buflen,
#ifdef LOW_PRECISION
			8
#else
			18
#endif
			,pu);
	}
	ret = buf;
	delete [] buf;
	return ret;
}

#endif //#if !defined(NO_STD_STRING) && !defined(NO_PRINTF)
#endif //#ifndef NO_TEXT

#if !defined(YES_CONSTEXPR)
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
#endif //#if !defined(YES_CONSTEXPR) || defined(NO_INLINE)
// Otherwise * and / are constexpr in header

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

#if !defined(YES_CONSTEXPR) // || defined(NO_INLINE)
PhysicalQuantity PhysicalQuantity::operator* (PhysicalQuantity::num rhs) const
{
	PhysicalQuantity ret(*this);
	ret.value = value * rhs;
	return ret;
}

PhysicalQuantity PhysicalQuantity::operator/ (PhysicalQuantity::num rhs) const
{
	PhysicalQuantity ret(*this);
	ret.value = value / rhs;
	return ret;
}
#endif //#if !defined(YES_CONSTEXPR) || defined(NO_INLINE)

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

#ifndef NO_TEXT
#ifndef PQ_GENCODE
bool PhysicalQuantity::findUnit(CSubString name, PhysicalQuantity::unitIndex_t& outUnitIndex, PhysicalQuantity::prefixIndex_t& outPrefixIndex)
{
	prefixIndex_t iPrefix = -1;
	unitIndex_t iUnit = 0;
	int nlen = name.length();
	if (nlen <= 0) { return false; }
	csubstr possibleUnitPart;
	int foundPrefixLen = 0;
	int foundUnitLen = 0;

#ifndef NO_HASHING
	static cstrHasherTiny hashPrefixSymbol(PQ::hashTableSeed_PrefixSymbols);
	static cstrHasherTiny hashUnitSymbol(PQ::hashTableSeed_UnitSymbols);
#ifndef NO_LONG_NAMES
	static cstrHasherTiny hashUnitLongName(PQ::hashTableSeed_UnitLongNames);
	static cstrHasherTiny hashUnitPlural(PQ::hashTableSeed_UnitPlurals);
	unsigned int hashLongName, hashPlural;
#endif //#ifndef NO_LONG_NAMES
	unsigned int iBucket, hashRaw, hashSymbol;

	char firstLetter[2];

#ifdef ROM_READ_BYTE
	char kubuf[sizeof(UnitDefinition)];
	const UnitDefinition& ku = *(UnitDefinition*)kubuf;
	Prefix preval;
#endif


	if ((nlen > 2) && (name[0] == 'd') && (name[1] == 'a'))
	{
		possibleUnitPart = csubstr(name, 2);
		hashRaw = (unsigned int)hashUnitSymbol(possibleUnitPart);
		hashSymbol = hashRaw % hashTableSize_UnitSymbols;
#ifdef ROM_READ_BYTE
		UnitSymbols_HashTableEntry_t usent;
		romcpy(&usent, &UnitSymbols_HashTable[hashSymbol], sizeof(usent));
#endif
		for (iBucket = 0; iBucket < UnitSymbols_HashTable[hashSymbol].bucketSize; iBucket++)
		{
#ifdef ROM_READ_BYTE
			romcpy(kubuf, &(KnownUnits[usent.bucket[iBucket]]), sizeof(UnitDefinition));
#endif
			if (possibleUnitPart == IFROM(
				KnownUnits[UnitSymbols_HashTable[hashSymbol].bucket[iBucket]].symbol,
				((UnitDefinition*)kubuf)->symbol))
			{
				outPrefixIndex = dekaIndex;
				outUnitIndex = IFROM(UnitSymbols_HashTable[hashSymbol].bucket[iBucket], usent.bucket[iBucket]);
				return true;
			}
		}

#ifndef NO_LONG_NAMES
		hashRaw = (unsigned int)hashUnitLongName(possibleUnitPart);
		hashLongName = hashRaw % hashTableSize_UnitLongNames;
#ifdef ROM_READ_BYTE
		UnitLongNames_HashTableEntry_t ulent;
		romcpy(&ulent, &UnitLongNames_HashTable[hashLongName], sizeof(ulent));
#endif
		for (iBucket = 0; iBucket < UnitLongNames_HashTable[hashLongName].bucketSize; iBucket++)
		{
#ifdef ROM_READ_BYTE
			romcpy(kubuf, &(KnownUnits[ulent.bucket[iBucket]]), sizeof(UnitDefinition));
#endif
			if (possibleUnitPart == IFROM(
				KnownUnits[UnitLongNames_HashTable[hashLongName].bucket[iBucket]].longName,
				((UnitDefinition*)kubuf)->longName))
			{
				outPrefixIndex = dekaIndex;
				outUnitIndex = IFROM(UnitLongNames_HashTable[hashLongName].bucket[iBucket], ulent.bucket[iBucket]);
				return true;
			}
		}

		hashRaw = (unsigned int)hashUnitPlural(possibleUnitPart);
		hashPlural = hashRaw % hashTableSize_UnitPlurals;
#ifdef ROM_READ_BYTE
		UnitPlurals_HashTableEntry_t upent;
		romcpy(&upent, &UnitPlurals_HashTable[hashLongName], sizeof(upent));
#endif
		for (iBucket = 0; iBucket < UnitPlurals_HashTable[hashPlural].bucketSize; iBucket++)
		{
#ifdef ROM_READ_BYTE
			romcpy(kubuf, &(KnownUnits[upent.bucket[iBucket]]), sizeof(UnitDefinition));
#endif
			if (possibleUnitPart == IFROM(KnownUnits[UnitPlurals_HashTable[hashPlural].bucket[iBucket]].plural,
				((UnitDefinition*)kubuf)->plural))
			{
				outPrefixIndex = dekaIndex;
				outUnitIndex = IFROM(UnitPlurals_HashTable[hashPlural].bucket[iBucket], upent.bucket[iBucket]);
				return true;
			}
		}
#endif // #ifndef NO_LONG_NAMES
	} // deka

	firstLetter[0] = name[0];
	firstLetter[1] = 0;
	hashSymbol = (unsigned int)hashPrefixSymbol(firstLetter) % hashTableSize_PrefixSymbols;
#ifdef ROM_READ_BYTE
	PrefixSymbols_HashTableEntry_t psent;
	romcpy(&psent, &PrefixSymbols_HashTable[hashSymbol], sizeof(psent));
#endif
	for (iBucket = 0; iBucket < PrefixSymbols_HashTable[hashSymbol].bucketSize; iBucket++)
	{
#ifdef ROM_READ_BYTE
		romcpy(&preval, &(KnownPrefixes[psent.bucket[iBucket]]), sizeof(Prefix));
		char firstchartest = ROM_READ_BYTE(preval.symbol);
		if (name[0] == firstchartest)
#else
		if (name[0] == KnownPrefixes[PrefixSymbols_HashTable[hashSymbol].bucket[iBucket]].symbol[0])
#endif
		{
			iPrefix = IFROM(PrefixSymbols_HashTable[hashSymbol], psent).bucket[iBucket];
			foundPrefixLen = (int) IFROM(strlen(KnownPrefixes[iPrefix].symbol), romstrlen(preval.symbol));
			break;
		}
	}

	csubstr tryUnitNames[2];
	tryUnitNames[0] = csubstr(name);
	tryUnitNames[1] = csubstr(name, 1);
	unitIndex_t iTryUnitName = 0;

	// look up unit symbol
	while(iTryUnitName < 2)
	{
		hashRaw = (unsigned int)hashUnitSymbol(tryUnitNames[iTryUnitName]);
		hashSymbol = hashRaw % hashTableSize_UnitSymbols;
#ifdef ROM_READ_BYTE
		UnitSymbols_HashTableEntry_t usent;
		romcpy(&usent, &UnitSymbols_HashTable[hashSymbol], sizeof(usent));
#endif
		for (iBucket = 0; iBucket < UnitSymbols_HashTable[hashSymbol].bucketSize; iBucket++)
		{
#ifdef ROM_READ_BYTE
			romcpy(kubuf, &KnownUnits[usent.bucket[iBucket]], sizeof(UnitDefinition));
#endif
			if (tryUnitNames[iTryUnitName] == IFROM(KnownUnits[UnitSymbols_HashTable[hashSymbol].bucket[iBucket]].symbol,
				ku.symbol))
			{
				iUnit = IFROM(UnitSymbols_HashTable[hashSymbol].bucket[iBucket], usent.bucket[iBucket]);
				foundUnitLen = IFROM((int)strlen(KnownUnits[iUnit].symbol), (int)romstrlen(ku.symbol));
				break;
			}
		}

#ifndef NO_LONG_NAMES
		if (foundUnitLen > 0) { break; }
		hashRaw = (unsigned int)hashUnitLongName(tryUnitNames[iTryUnitName]);
		hashLongName = hashRaw % hashTableSize_UnitLongNames;
#ifdef ROM_READ_BYTE
		UnitLongNames_HashTableEntry_t ulent;
		romcpy(&ulent, &UnitLongNames_HashTable[hashLongName], sizeof(ulent));
#endif
		for (iBucket = 0; iBucket < IFROM(UnitLongNames_HashTable[hashLongName].bucketSize, ulent.bucketSize); iBucket++)
		{
		#ifdef ROM_READ_BYTE
			romcpy(kubuf, &KnownUnits[ulent.bucket[iBucket]], sizeof(UnitDefinition));
		#endif
			if (tryUnitNames[iTryUnitName] == IFROM(KnownUnits[UnitLongNames_HashTable[hashLongName].bucket[iBucket]].longName,	ku.longName))
			{
				iUnit = IFROM(UnitLongNames_HashTable[hashLongName].bucket[iBucket], ulent.bucket[iBucket]);
				foundUnitLen = (int) IFROM(strlen(KnownUnits[iUnit].longName), romstrlen(ku.longName));
				break;
			}
		}
		//if (iUnit != -1) { break; }
		if (foundUnitLen > 0) { break; }

		hashRaw = (unsigned int)hashUnitPlural(tryUnitNames[iTryUnitName]);
		hashPlural = hashRaw % hashTableSize_UnitPlurals;
#ifdef ROM_READ_BYTE
		UnitPlurals_HashTableEntry_t upent;
		romcpy(&upent, &UnitPlurals_HashTable[hashPlural], sizeof(upent));
#endif
		for (iBucket = 0; iBucket < IFROM(UnitPlurals_HashTable[hashPlural], upent).bucketSize; iBucket++)
		{
#ifdef ROM_READ_BYTE
			romcpy(kubuf, &KnownUnits[upent.bucket[iBucket]], sizeof(UnitDefinition));
#endif
			if (tryUnitNames[iTryUnitName] == IFROM(KnownUnits[UnitPlurals_HashTable[hashPlural].bucket[iBucket]].plural, ku.plural))
			{
				iUnit = IFROM(UnitPlurals_HashTable[hashPlural].bucket[iBucket], upent.bucket[iBucket]);
				foundUnitLen = (int) IFROM(strlen(KnownUnits[iUnit].plural), romstrlen(ku.plural));
				break;
			}
		}
		//if (iUnit != -1) { break; }
		if (foundUnitLen > 0) { break; }

#endif //#ifndef NO_LONG_NAMES

		iTryUnitName++;
	}


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


#endif // #else //#ifndef NO_HASHING

	//csubstr ss(nullptr, 0, 0);
	//if (iUnit == -1 || (foundUnitLen < nlen))
	//if (foundUnitLen == 0 || (foundUnitLen < nlen))
	romcsubstr sy, lo, pl;
	if (foundUnitLen < nlen)
	{
		// Nothing found so far, revert to slow linear search
		if (foundUnitLen == 0)
		{
			for (unitIndex_t i = 0; i < KnownUnitsLength; i++)
			{
				sy = KnownUnits[i].symbol; //, 0, foundUnitLen);
				if (sy.length() > 0 && sy.ends(name))
				{
					iUnit = i;
					foundUnitLen = sy.length();
					break;
				}

#ifndef NO_LONG_NAMES
				lo = KnownUnits[i].longName;
				if (lo.length() > 0 && lo.ends(name))
				{
					iUnit = i;
					foundUnitLen = lo.length();
					break;
				}

				pl = KnownUnits[i].plural;
				if (pl.length() > 0 && pl.ends(name))
				{
					iUnit = i;
					foundUnitLen = pl.length();
					break;
				}
#endif //#ifndef NO_LONG_NAMES

			} // for in KnownUnits
		} // if (foundUnitLen == 0)
		if (foundUnitLen == 0) { return false; }

		csubstr prefixToFind(name, 0, name.length() - foundUnitLen);
		for (prefixIndex_t i = 0; i < KnownPrefixesLength; i++)
		{
			if (prefixToFind == romcsubstr(KnownPrefixes[i].symbol)
#ifndef NO_LONG_NAMES
			 ||	prefixToFind == romcsubstr(KnownPrefixes[i].longName)
#endif
			 )
			{
				iPrefix = i;
				foundPrefixLen = prefixToFind.length();
				break;
			}
		}
	}
// #endif  //#ifndef NO_HASHING

	if (foundUnitLen == 0) { return false; }
	if (foundUnitLen == nlen)
	{
		outPrefixIndex = -1;
		outUnitIndex = iUnit;
		return true;
	}
	else if ((foundPrefixLen + foundUnitLen == nlen)
		&& ((KnownUnits[iUnit].flags & NOPREFIX) == 0))
	{
		outPrefixIndex = iPrefix;
		outUnitIndex = iUnit;
		return true;
	}
	return false;
}
#else //#ifndef PQ_GENCODE
bool PhysicalQuantity::findUnit(CSubString name, PhysicalQuantity::unitIndex_t& outUnitIndex, PhysicalQuantity::prefixIndex_t& outPrefixIndex)
{
#ifdef NO_THROW
	errorHandler(errorUserContext, E_HEADER_CONFIG);
#else
	throw HeaderConfigException("findUnit() is undefined while PQ_GENCODE is defined");
#endif
	return false;
}
#endif //#else of #ifndef PQ_GENCODE
#endif //#ifndef NO_TEXT

bool PhysicalQuantity::operator==(const PhysicalQuantity& rhs) const
{
	if (memcmp(dim, rhs.dim, sizeof(dim)) != 0) { return false; }
	return PhysicalQuantity::feq(value, rhs.value, equalityToleranceFactor);
}
bool PhysicalQuantity::operator!=(const PhysicalQuantity& rhs) const
{
	return PhysicalQuantity::feq(value, rhs.value, equalityToleranceFactor);
}
bool PhysicalQuantity::operator>=(const PhysicalQuantity& rhs) const
{
	if (memcmp(dim, rhs.dim, sizeof(dim)) != 0) { return false; }
	return value >= rhs.value;
}
bool PhysicalQuantity::operator<=(const PhysicalQuantity& rhs) const
{
	if (memcmp(dim, rhs.dim, sizeof(dim)) != 0) { return false; }
	return value <= rhs.value;
}


//============================================================================================
// Display and conversion stuff

#ifndef NO_TEXT
void PhysicalQuantity::UnitListBase::build(const CSubString& unitList, PhysicalQuantity::UnitListBase::UnitPref* buffer, int bufferSizeBytes, bool dynamic)   //(const CSubString& unitList, int* buffer, int bufferLen, bool dynamic)
{
	int len = (int)unitList.length();
	int pos = 0;
	unitIndeces = buffer;
	int storageCapacity = bufferSizeBytes / sizeof(UnitPref);
	count_ = 0;
	int wordStart = 0;
	int wordEnd = 0;
	csubstr word;
	while (unitList[wordStart] == ' ') { wordStart++; }
	char c = 0;
	unitIndex_t unitIndex;
	prefixIndex_t prefixIndex;
	int iCaret;
	for (pos = wordStart; pos <= len; pos++)
	{
		if (pos < len) { c = unitList[pos]; }
		else {c = 0; }
		if (pos >= len || c == ' ')
		{
			wordEnd = pos;
			word = csubstr(unitList, wordStart, wordEnd - wordStart);
			iCaret = word.find_first_of('^');
			if (iCaret != -1) { word = word.substr(0, iCaret); }

			if (PhysicalQuantity::findUnit(word, unitIndex, prefixIndex))
			{
				if (count_ >= storageCapacity)
				{
#ifdef NO_NEW
					if (dynamic) {} // squash compiler warnings about unused parameters
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

PhysicalQuantity::UnitListBase::~UnitListBase()
{
}

#ifndef NO_NEW

PhysicalQuantity::UnitList_dynamic::UnitList_dynamic(const CSubString& unitList)
{
	unitIndeces = nullptr;
	build(unitList, nullptr, 0, true);
}
PhysicalQuantity::UnitList_dynamic::~UnitList_dynamic()
{
	if (unitIndeces) 
	{
		delete [] unitIndeces; 
	}
}

#endif // #ifndef NO_NEW

PhysicalQuantity::UnitList_static::UnitList_static(const CSubString& unitList, void* storage, size_t storageSizeBytes)
{
	count_ = 0;
	unitIndeces = (UnitListBase::UnitPref*)storage;
	dynamic = false;
	int capacity = (int)(storageSizeBytes / sizeof(UnitListBase::UnitPref));
	build(unitList, unitIndeces, capacity, false);
}


PhysicalQuantity::UnitList_static::UnitList_static(const CSubString& unitList_SpaceDelimited)
{
	count_ = 0;
	unitIndeces = nullptr;
	dynamic = false;
	if (unitList_SpaceDelimited.length() > 0)
	{
#ifdef NO_THROW
		errorHandler(errorUserContext, E_MEMORY);
#else
		throw bad_alloc(); //"Trying to initialize a non empty unit list with no buffer.");
#endif
	}
}


const PhysicalQuantity::UnitListBase::UnitPref& PhysicalQuantity::UnitListBase::operator[] (int i) const
{
	return unitIndeces[i];
}

//PhysicalQuantity PhysicalQuantity::eval(CSubString str)
//{
//	return eval1(str);
//	char c = 0;
//	char prev = 0;
//	PQ acc = 0.0; // accumulator
//	for (int i = 0; i < str.length(); i++)
//	{
//		prev = c;
//		c = str[i];
//
//	}
//}

PhysicalQuantity PhysicalQuantity::eval(CSubString str)
{
	str = str.trim();
	int pleft = -1;
	int pright = -1;
	int plevel = 0;
	PQ left;
	PQ right;
	PQ ret;
	char c;
	int len = str.length();
	plevel = 0;
	int rightmostNonSpace = -1;
	csubstr leftstr;
	csubstr rightstr;

	for (int i = len - 1; i >= 0; i--)
	{
		c = str[i];
		leftstr = str.substr(0, i);
		rightstr = str.substr(i + 1);

		if (c != ' ' && rightmostNonSpace == -1) { rightmostNonSpace = i; }
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
		else if (c == '+' && plevel == 0 && str[i - 1] != 'e' && str[i - 1] != 'E')
		{
			//left = eval(str.substr(0, i));
			//right = eval(str.substr(i + 1));
			//return left + right;
			DEBUG_LR_STR(leftstr, rightstr, "+");
			left = eval(leftstr);
			right = eval(rightstr);
			ret = left + right;
			DEBUG_LR_RESULT(left, right, "+", ret);
			return ret;
		}
		else if (c == '-' && plevel == 0 && str[i - 1] != 'e' && str[i - 1] != 'E')
		{
			//left = eval(str.substr(0, i));
			//right = eval(str.substr(i + 1));
			//return left - right;
			DEBUG_LR_STR(leftstr, rightstr, "-");
			left = eval(leftstr);
			right = eval(rightstr);
			ret = left - right;
			DEBUG_LR_RESULT(left, right, "-", ret);
			return ret;
		}
	}

	plevel = 0;
	for (int i = len - 1; i >= 0; i--)
	{
		c = str[i];
		leftstr = str.substr(0, i);
		rightstr = str.substr(i + 1);
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
		//else if ((c == '*' || c == ' ') && plevel == 0)
		else if (c == '*' && plevel == 0)
		{
			//left = eval(str.substr(0, i));
			//right = eval(str.substr(i + 1));
			//return left * right;
			DEBUG_LR_STR(leftstr, rightstr, "*");
			left = eval(leftstr);
			right = eval(rightstr);
			ret = left * right;
			DEBUG_LR_RESULT(left, right, "*", ret);
			return ret;
		}
		else if (c == '/' && plevel == 0)
		{
			//left = eval(str.substr(0, i));
			//right = eval(str.substr(i + 1));
			//return left / right;
			DEBUG_LR_STR(leftstr, rightstr, "/");
			left = eval(leftstr);
			right = eval(rightstr);
			ret = left / right;
			DEBUG_LR_RESULT(left, right, "/", ret);
			return ret;
		}
	}

	plevel = 0;
	for (int i = len - 1; i >= 0; i--)
	{
		c = str[i];
		leftstr = str.substr(0, i);
		rightstr = str.substr(i + 1);
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
			DEBUG_LR_STR(leftstr, rightstr, "^");
			left = eval(leftstr);
			right = eval(rightstr);

			PQ mant = left; //eval(str.substr(0, i));
			const PQ& exp = right; //(str.substr(i + 1));
			for (int id = 0; id < (int)QuantityType::ENUM_MAX; id++)
			{
				if (exp.dim[id] != 0)
				{
#ifdef NO_THROW
					errorHandler(errorUserContext, E_INVALID_EXPRESSION);
					return 0.0;
#else
					throw InvalidExpressionException("Exponent must be a scalar");
#endif
				}
			}
			if (exp.value != (num)((int)exp.value))
			{
				for (int id = 0; id < (int)QuantityType::ENUM_MAX; id++)
				{
					if (mant.dim[id] != 0)
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
			mant.value = ::pow(mant.value, exp.value);
			signed char newdim[(int)QuantityType::ENUM_MAX];
			for (int id = 0; id < (int)QuantityType::ENUM_MAX; id++)
			{
				signed int newpow = (signed int)mant.dim[id] * (signed int)exp.value;
				if (newpow > 127 || newpow < -128)
				{
#ifdef NO_THROW
					errorHandler(errorUserContext, E_OVERFLOW);
					return 0.0;
#else
					throw overflow_error("Unit exponent too large");
#endif
				}
				newdim[id] = (signed char)newpow;
			}
			memcpy(mant.dim, newdim, sizeof(newdim));
			DEBUG_LR_RESULT(left, right, "^", mant);
			return mant;
		}
	}


	if (plevel != 0 ||
		(pleft == -1 && pright != -1) ||
		(pleft != -1 && pright == -1))
	{
#ifdef NO_THROW
		errorHandler(errorUserContext, E_INVALID_EXPRESSION);
		return 0.0;
#else
		throw InvalidExpressionException("Mismatched parentheses");
#endif
	}


	if (pleft == -1)
	{
		return PQ(str);
	}
	//left = eval(str.substr(pleft + 1, pright - pleft - 1));
	leftstr = str.substr(pleft + 1, pright - pleft - 1);
	left = eval(leftstr);
	if (rightmostNonSpace > pright)
	{
		//right = eval(str.substr(pright + 1));
		//return left * right;
		rightstr = str.substr(pright + 1);
		DEBUG_LR_STR(leftstr, rightstr, "(implicit)*");
		right = eval(rightstr);
		ret = left * right;
		DEBUG_LR_RESULT(left, right, "(implicit)*", ret);
		return ret;
	}
	else 
	{
		return left;
	}
}

size_t PhysicalQuantity::sprint(char* buf, size_t size, unsigned int precision, const PhysicalQuantity::CSubString& sspu, bool useSlash) const
{
	return sprint(buf, size, precision, UnitList(sspu), useSlash);
}

size_t PhysicalQuantity::sprint(char* buf, size_t size, unsigned int precision, const CSubString& sspu, void* puBuf, size_t puBufSize, bool useSlash) const
{
	return sprint(buf, size, precision, UnitList_static(sspu, puBuf, puBufSize), useSlash);
}
#endif // #ifndef NO_TEXT

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

bool PhysicalQuantity::writeNetworkBinary(void* buf)
{
	short endianTest = 1;
	if (*(char*)(&endianTest) == 1)
	{
		// Little endian
		for (size_t i = 0; i < sizeof(num); i++)
		{
			*((char*)(buf) + i) = *((char*)(&value) + (sizeof(num) - i - 1));
		}
	}
	else
	{
		// Big endian
		*(num*)buf = value;
	}
	memcpy((char*)buf + sizeof(num), dim, sizeof(dim));
	return true;
}

bool PhysicalQuantity::readNetworkBinary(void* buf)
{
	short endianTest = 1;
	if (*(char*)(&endianTest) == 1)
	{
		for (size_t i = 0; i < sizeof(num); i++)
		{
			*((char*)(&value) + i) = *((char*)(buf) + sizeof(num) - i - 1);
		}
	}
	memcpy(dim, (char*)buf + sizeof(num), sizeof(dim));
	return true;
}

PhysicalQuantity PhysicalQuantity::pow(int x)
{
	PQ ret;
	ret.value = ::pow(value, x);
	for (int pi = 0; pi < (int)QuantityType::ENUM_MAX; pi++)
	{
		ret.dim[pi] = dim[pi] * x;
	}
	return ret;
}

bool PhysicalQuantity::isScalar()
{
	for (int i = 0; i < (int)QuantityType::ENUM_MAX; i++)
	{
		if (dim[i] != 0) { return false; }
	}
	return true;
}

PhysicalQuantity::num PhysicalQuantity::getScalar()
{
	if (!isScalar())
	{
#ifdef NO_THROW
		errorHandler(errorUserContext, E_UNIT_MISMATCH);
		return 0;
#else
		throw UnitMismatchException("Attempting to read a scalar but value has units.");
#endif
	}
	return value;
}

#ifdef NO_SPRINTF_FLOAT
size_t PhysicalQuantity::printNum(char* buf, size_t size, PhysicalQuantity::num val, unsigned int precision)
{
	unsigned int iOut = 0;
	if (iOut == size - 1) { buf[size - 1] = 0; return size; }
	if (val < 0)
	{
		buf[iOut++] = '-';
		if (iOut == size - 1) { buf[size - 1] = 0; return size; }
		val *= -1.0;
	}
	int exp = 1;
	num expCompare = 10;
	while (val > expCompare)
	{
		exp++;
		expCompare *= 10.0;
	}
	while (val < expCompare)
	{
		exp--;
		expCompare /= 10.0;
	}
	if (iOut + 2 == size - 1) { buf[size - 1] = 0; return iOut + 2; }
	val /= expCompare;
	buf[iOut++] = '0' + ((char)((int)val));
	buf[iOut++] = '.';
	val = (val - ((int)val)) * 10.0;
	while (val > 0.0 && precision > 0)
//#ifdef LOW_PRECISION
//		10
//#else
//		20
//#endif
//		)
	{
		buf[iOut++] = '0' + ((char)((int)val));
		if (iOut == size - 1) { buf[size - 1] = 0; return size; }
		val = (val - ((int)val)) * 10.0;
		precision--;
	} 
	if (val > 5.0)
	{
		// round
		for (int ci = iOut - 1; ci >= 0; ci--)
		{
			if (buf[ci] >= '0' && buf[ci] <= '8')
			{
				(buf[ci])++;
				break;
			}
			else if (buf[ci] == '9')
			{
				buf[ci] = '0';
				iOut--;
			}
		}
	}


	if (exp != 0)
	{
		buf[iOut++] = 'e';
		if (iOut + 4 >= size) { buf[iOut] = 0; return iOut + 4; }
		sprintf(buf + iOut, "%d", exp);
		iOut += (int)strlen(buf + iOut);
	}
	buf[iOut++] = 0;
	return iOut;
}
#else
size_t PhysicalQuantity::printNum(char* buf, size_t size, PhysicalQuantity::num val, unsigned int precision)
{
	char formatbuf[20];
	sprintf(formatbuf, "%%.%ug", precision);
	sprintf(buf, formatbuf, val);
	return strlen(buf);
}
#endif


#ifdef NO_INLINE
PhysicalQuantity PhysicalQuantity::get1kg() { signed char d[5]={1,0,0,0,0}; return PhysicalQuantity(1.0, d); }
PhysicalQuantity PhysicalQuantity::get1m() {  signed char d[5]={0,1,0,0,0}; return PhysicalQuantity(1.0, d); }
PhysicalQuantity PhysicalQuantity::get1s() {  signed char d[5]={0,0,1,0,0}; return PhysicalQuantity(1.0, d); }
PhysicalQuantity PhysicalQuantity::get1K() {  signed char d[5]={0,0,0,1,0}; return PhysicalQuantity(1.0, d); }
PhysicalQuantity PhysicalQuantity::get1A() {  signed char d[5]={0,0,0,0,1}; return PhysicalQuantity(1.0, d); }
PhysicalQuantity operator*(PhysicalQuantity::num left, const PhysicalQuantity& right) { return PhysicalQuantity(left) * right; }
PhysicalQuantity operator/(PhysicalQuantity::num left, const PhysicalQuantity& right) { return PhysicalQuantity(left) / right; }
PhysicalQuantity operator+(PhysicalQuantity::num left, const PhysicalQuantity& right) { return PhysicalQuantity(left) + right; }
PhysicalQuantity operator-(PhysicalQuantity::num left, const PhysicalQuantity& right) { return PhysicalQuantity(left) - right; }
#endif //#ifdef NO_INLINE



#ifndef NO_TEXT
#ifndef NO_HASHING
PhysicalQuantity::cstrHasherTiny::cstrHasherTiny()
{
	seed = PhysicalQuantity::defaultHashSeed;
}
PhysicalQuantity::cstrHasherTiny::cstrHasherTiny(size_t seed_p)
{
	seed = seed_p;
}
#endif //#ifndef NO_HASHING
#endif //#ifndef NO_TEXT

#ifdef NO_INLINE
#ifdef NO_THROW
void PhysicalQuantity::SetErrorHandler(void (*errorHandler_arg)(void* context, ErrorCode e)) { errorHandler = errorHandler_arg; }
#endif //#ifdef NO_THROW

int PhysicalQuantity::UnitListBase::count() const { return count_; }
//bool PhysicalQuantity::unitsMatch(const PhysicalQuantity& rhs) const { return isLikeQuantity(rhs); }

#ifndef NO_TEXT
PhysicalQuantity::PhysicalQuantity(const char* str) { PhysicalQuantity(CSubString(str)); }
void PhysicalQuantity::parseUnits(const char* unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut) { return parseUnits(CSubString(unitStr), unitsOut, factorOut, offsetOut); }
PhysicalQuantity::num PhysicalQuantity::convert(const char* units) const { return convert(csubstr(units)); }
bool PhysicalQuantity::findUnit(const char* pcharName, unitIndex_t& outUnitIndex, prefixIndex_t& outPrefixIndex) { return findUnit(CSubString(pcharName), outUnitIndex, outPrefixIndex); }
void PhysicalQuantity::parse(const char* text) { parse(csubstr(text)); }
//size_t PhysicalQuantity::sprint(char* buf, int size, const char* pu, bool useSlash) const { return sprint(buf, size, UnitList(pu), useSlash); }
size_t PhysicalQuantity::sprint(char* buf, int size, unsigned int precision, bool useSlash) const { return sprint(buf, size, precision, UnitList(""), useSlash); }
#endif //#ifndef NO_TEXT

#ifndef NO_HASHING
size_t PhysicalQuantity::cstrHasherTiny::operator()(const char* s) const { return operator()(CSubString(s)); }
#endif //#ifndef NO_HASHING

#endif //#ifdef NO_INLINE


