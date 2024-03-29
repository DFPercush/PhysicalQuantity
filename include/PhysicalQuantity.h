#pragma once

#ifdef NO_SKETCH
#include <PhysicalQuantity/ppOptions.h>
#else
#include "ppOptions.h"
#endif


//==================================================================================
// Config checking
#ifdef NO_TEXT
#define NO_HASHING
#endif

#ifndef DECLARE_CONST_ARRAY
#if defined(ARDUINO)
#define DECLARE_CONST_ARRAY(type, name) const PROGMEM type name[]
#else
#define DECLARE_CONST_ARRAY(type, name) const type name[]
#endif
#endif

#ifndef DECLARE_CONST
#if defined(ARDUINO)
#define DECLARE_CONST(type, name) const PROGMEM type name
#else
#define DECLARE_CONST(type, name) const type name
#endif
#endif

#if defined(ARDUINO) && !defined(NO_SPRINTF_FLOAT)
#define	NO_SPRINTF_FLOAT
#endif


// Set compiled options flags

#ifdef NO_NEW
#define PQ_HAS_NO_NEW 0x01
#define NO_STD_STRING
#else
#define PQ_HAS_NO_NEW 0
#endif
#ifdef NO_STD_STRING
#define PQ_HAS_NO_STD_STRING 0x02
#else
#define PQ_HAS_NO_STD_STRING 0
#endif
#ifdef NO_LITERALS
#define PQ_HAS_NO_LITERALS 0x04
#else
#define PQ_HAS_NO_LITERALS 0
#endif
#ifdef NO_INLINE
#define PQ_HAS_NO_INLINE 0x08
#else
#define PQ_HAS_NO_INLINE 0
#endif
#ifdef NO_HASHING
#define PQ_HAS_NO_HASHING 0x10
#else
#define PQ_HAS_NO_HASHING 0
#endif
#ifdef NO_THROW
#define PQ_HAS_NO_THROW 0x20
#else
#define PQ_HAS_NO_THROW 0
#endif
// 0x80 is free
//#ifdef YES_CONSTEXPR
//#define PQ_HAS_CONSTEXPR 0x80
//#else
//#define PQ_HAS_CONSTEXPR 0
//#endif
#ifdef NO_LONG_NAMES
#define PQ_HAS_NO_LONG_NAMES 0x100
#else
#define PQ_HAS_NO_LONG_NAMES 0
#endif
#ifdef NO_TEXT
#define PQ_HAS_NO_TEXT 0x200
#else
#define PQ_HAS_NO_TEXT 0
#endif
#ifndef NO_CONSTEXPR
#define PQ_HAS_NO_CONSTEXPR 0x400
#else
#define PQ_HAS_NO_CONSTEXPR 0
#endif
#ifdef ROM_READ_BYTE
#define PQ_HAS_USE_ROM_ACCESSOR 0x800
#else
#define PQ_HAS_USE_ROM_ACCESSOR 0
#endif
#ifdef LOW_PRECISION
#define PQ_HAS_LOW_PRECISION 0x1000
#else
#define PQ_HAS_LOW_PRECISION 0
#endif
#ifdef PQ_DEBUG_EVAL
#define PQ_HAS_DEBUG_EVAL 0x2000
#else
#define PQ_HAS_DEBUG_EVAL 0
#endif
#ifdef NO_SPRINTF_FLOAT
#define PQ_HAS_NO_SPRINTF_FLOAT 0x4000
#else
#define PQ_HAS_NO_SPRINTF_FLOAT 0
#endif
// Typedefs wouldn't affect anything at the linker/binary stage, would they?
//#ifdef NO_TYPEDEFS
//#define PQ_HAS_NO_TYPEDEFS 0xxxxxx
//#else
#define PQ_HAS_NO_TYPEDEFS 0
//#endif

#define PQ_HEADER_OPTIONS (PQ_HAS_NO_NEW | PQ_HAS_NO_STD_STRING | PQ_HAS_NO_LITERALS | \
	PQ_HAS_NO_INLINE | PQ_HAS_NO_HASHING | PQ_HAS_NO_THROW | PQ_HAS_NO_TYPEDEFS | \
	PQ_HAS_NO_LONG_NAMES | PQ_HAS_NO_TEXT | PQ_HAS_NO_CONSTEXPR | PQ_HAS_USE_ROM_ACCESSOR | \
	PQ_HAS_LOW_PRECISION | PQ_HAS_DEBUG_EVAL | PQ_HAS_NO_SPRINTF_FLOAT)

// End config checking
//==================================================================================



#ifndef NO_CONSTEXPR
#define YES_CONSTEXPR
#endif

#if !defined(INLINE_KEYWORD)
#if !defined(NO_INLINE)
#define INLINE_KEYWORD __inline
#else
#define INLINE_KEYWORD
#endif
#endif

#if defined(PQ_GENCODE)
#ifdef DECLARE_CONST_ARRAY
#undef DECLARE_CONST_ARRAY
#endif
#ifdef DECLARE_CONST
#undef DECLARE_CONST
#endif
#define DECLARE_CONST_ARRAY(type, name) const type name[]
#define DECLARE_CONST(type, name) const type name
#ifdef _MSC_VER
#pragma warning(default:4005)
#endif
#endif


#ifndef DEFINE_CONST_ARRAY
#define DEFINE_CONST_ARRAY(type, name) DECLARE_CONST_ARRAY(type, name)
#endif

#ifndef DEFINE_CONST
#define DEFINE_CONST(type, name) DECLARE_CONST(type, name)
#endif

#if defined(PQ_DEBUG_EVAL) && defined(NO_STD_STRING)
#undef PQ_DEBUG_EVAL
#endif

// Should be getting this from ppOptions.h, but it really, *really* needs to be defined for sure
#ifndef MAX_NUM_TEXT_LENGTH
#define MAX_NUM_TEXT_LENGTH 26
#endif

//#ifdef ROM_READ_BYTE
//#ifdef ARDUINO	
//#define ROMLITERAL(str) F(str)
//#else
//#define ROMLITERAL(str) str
//#endif
//char PQ_DefaultRomAccessor(char* addr);
//#endif


//========================================
// Standard library dependencies
#ifndef NO_THROW
#include <exception>
#include <stdexcept>
#endif
#ifdef NO_NEW
#define NO_STD_STRING
#endif
#ifndef NO_STD_STRING
#include <string>
#endif

#include <array>

#if !defined(NO_STD_MAP) && !defined(NO_NEW) && !defined(NO_STD_STRING) && !defined(NO_TEXT)
#define PQVARS
#include <unordered_map>
#endif

#ifndef NO_STD_CHRONO
#include <chrono>
#ifndef _CHRONO_
#define _CHRONO_
#endif
#endif

// size_t
#include <stddef.h>

// abs(), pow()
#include <math.h>

// memcpy()
#include <memory.h>

// end standard library dependencies
//========================================


#ifdef ROM_READ_BYTE
void romcpy(void* to, const void* from, size_t size);
size_t romstrlen(const char* s);
int memcmp_romleft(const char* left, const char* right, size_t len);
int memcmp_romright(const char* left, const char* right, size_t len);
int memcmp_romboth(const char* left, const char* right, size_t len);
#endif

template <typename T> T rom(const T& v)
{
#ifdef ROM_READ_BYTE
	char buf[sizeof(T)];
	romcpy(buf, &v, sizeof(T));
	return *(T*)buf;
#else
	return v;
#endif
}


//###########################################################################################
// Main class
bool InitLibPQ(int headerOptionFlags = PQ_HEADER_OPTIONS);

#ifdef PQVARS
class PQVarList;
#endif



class PhysicalQuantity
{
public:

	//======================
	// Base numeric type
#ifndef PQ_NUM_TYPE
#define PQ_NUM_TYPE double
#endif
	typedef PQ_NUM_TYPE num;
	//typedef float num;
	//======================

#ifndef NO_TEXT

	//==================================================================================
	// CSubString
	class CSubString
	{
	protected:
		static char nullstr[];
		const char* str;
		int start;
		int end; // constructor takes a length to emulate standard behavior, but internally end is easier
	public:
#ifdef ROM_READ_BYTE
		bool rom;
#endif

#ifdef YES_CONSTEXPR
#ifdef ROM_READ_BYTE
		CSubString() : str{nullstr}, start{0}, end{0}, rom{false} {}
#else
		CSubString() : str{nullstr}, start{0}, end{0} {}
#endif
#else // #ifdef YES_CONSTEXPR
#ifdef NO_INLINE
		CSubString();
#else // #ifdef NO_INLINE

#ifdef ROM_READ_BYTE
		CSubString() { str = nullstr; start = 0; end = 0; rom = false; }
#else
		CSubString() { str = nullstr; start = 0; end = 0; }
#endif
#endif // #ifdef NO_INLINE / #else
#endif // #ifdef YES_CONSTEXPR / else

		CSubString(const char* str_arg, int start_arg = 0, int len_arg = -1);
		CSubString(const CSubString& from, int start_arg = 0, int len_arg = -1);

#ifndef NO_STD_STRING
		CSubString(const std::string& stdstr); // Be careful of object mutation and lifetime!
#endif

		CSubString& operator=(const CSubString& cp);
		bool operator== (const char* cmp) const;
		bool operator==(const CSubString& cmp) const;
		bool begins(const CSubString& test) const;
		bool ends(const CSubString& test) const;
		//TODO: int find(const char* test, int start = 0) const; // where this needle appears in a haystack argument, or -1
		bool copyTo(char* buf, int size) const;
		int find_first_of(const CSubString& find, int startOfs = 0) const;
		int find_first_not_of(const CSubString& find, int startOfs = 0) const;
		int find_first_of(char c, int startOfs = 0) const;
		int find_first_not_of(char c, int startOfs = 0) const;
		int find_last_of(const CSubString& find, int startOfs = -1) const;
		int find_last_not_of(const CSubString& find, int startOfs = -1) const;
		int find_last_of(char c, int startOfs = -1) const;
		int find_last_not_of(char c, int startOfs = -1) const;
		int atoi();
		double atof();
		CSubString substr(int startArg = 0, int lenArg = -1) const;
		bool isnumber() const;
		bool isint() const;
		CSubString trim() const;
		const char* getPtr() const { return &str[start]; }

#ifndef NO_STD_STRING
		std::string toStdString() const;
		operator std::string() const { return toStdString(); }
#endif

#ifdef NO_INLINE
		char operator[](int index) const;
		bool begins(const char* test) const;
		bool ends(const char* test) const;
		int size() const;
		int length() const;
		int find_first_of(const char* find, int startOfs = 0) const;
		int find_first_not_of(const char* find, int startOfs = 0) const;
		bool operator!= (const char* cmp) const;
		bool operator!=(const CSubString& cmp) const;
		void clear();
#else
		void clear() { str = nullstr; start = 0; end = 0; }
#ifdef ROM_READ_BYTE
		INLINE_KEYWORD char operator[](int index) const
		{
			if (index < 0 || start + index >= end) { return 0; }
			if (rom) { return ROM_READ_BYTE(str + start + index); }
			else { return str[start + index]; }
		}
#else
		INLINE_KEYWORD char operator[](int index) const { if (index < 0 || start + index >= end) { return 0; } return str[start + index];}
#endif
		INLINE_KEYWORD bool begins(const char* test) const { return begins(CSubString(test)); }
		INLINE_KEYWORD bool ends(const char* test) const { return ends(CSubString(test)); }
		INLINE_KEYWORD int size() const { return end - start; }
		INLINE_KEYWORD int length() const { return (end - start) >= 0 ? (end-start) : 0 ; }
		INLINE_KEYWORD int find_first_of(const char* find, int startOfs = 0) const { return find_first_of(CSubString(find), startOfs); }
		INLINE_KEYWORD int find_first_not_of(const char* find, int startOfs = 0) const { return find_first_not_of(CSubString(find), startOfs); }
		INLINE_KEYWORD bool operator!= (const char* cmp) const { return !(operator==(cmp)); }
		INLINE_KEYWORD bool operator!=(const CSubString& cmp) const { return !(operator==(cmp)); }

#endif
	};

#ifdef ROM_READ_BYTE
	class romcsubstr : public CSubString
	{
	public:
		romcsubstr() : CSubString() { rom = true; }
		romcsubstr(const char* str_arg, int start_arg = 0, int len_arg = -1); // : CSubString(str_arg, start_arg, len_arg) { rom = true; }
		romcsubstr(const CSubString& from, int start_arg = 0, int len_arg = -1) : CSubString(from, start_arg, len_arg) { rom = true; }
	};
#else
	typedef CSubString romcsubstr;
#endif
	// End CSubString
	//==================================================================================
#endif //#ifndef NO_TEXT


	//==================================================================================
	// Unit and dimension stuff
	enum class QuantityType // : int
	{
		MASS,
		DISTANCE,
		TIME,
		TEMPERATURE,
		CURRENT,
		ENUM_MAX
	};

	template <signed char TMa, signed char TDi,signed char TTi,signed char TTe,signed char TCu, unsigned char iofs>
	friend class SPQ;


#ifdef YES_CONSTEXPR
	static constexpr int ND = (int)QuantityType::ENUM_MAX;
#else
	static const int ND;
#endif

typedef unsigned char UnitFlags_t;

#if !defined(NO_TEXT) || defined(PQ_GENCODE)
	struct UnitDefinition
	{
		const char* symbol;
		//DECLARE_CONST_ARRAY(char, symbol);
#ifndef NO_LONG_NAMES
		const char* longName;
		const char* plural;
#endif
		num factor;
		const signed char dim[(int)QuantityType::ENUM_MAX];
		UnitFlags_t flags = 0; // watch size
	};
	enum UnitFlags
	{
		NOPREFIX = 0x01,
		CANPREFIX = 0,
		NOLITERAL = 0x02,
		MAKELITERAL = 0,
		NOBASELITERAL = 0x04, // Because some compilers (g++/arm 7.2.1 for example) have reserved literals like _N and _C
		EXPLICIT = 0x08 // Only output in these units if explicitly requested (*ahem* mph...)
	};
// PQ Number of Dimensions
#define PQND ((int)PQ::QuantityType::ENUM_MAX)
	static constexpr int NDIM = (int)QuantityType::ENUM_MAX;

	struct Prefix
	{
		const char* symbol;
#ifndef NO_LONG_NAMES
		const char* longName;
#endif
		num factor;
	};
	// End unit and dimension stuff
	//==================================================================================


#ifndef NO_HASHING
	typedef unsigned short bucketSize_t;

#ifdef PQ_GENCODE
	// Default hash table params are defined in hash.cpp
	// Primarily used by gencode if used without "optimize"
	static const int default_hashTableSize_UnitSymbols;
	static const int default_hashTableSize_UnitLongNames;
	static const int default_hashTableSize_PrefixSymbols;
	static const int default_hashTableSize_UnitPlurals;
	static const int defaultHashSeed_UnitSymbols;
	static const int defaultHashSeed_UnitLongNames;
	static const int defaultHashSeed_UnitPlurals;
	static const int defaultHashSeed_PrefixSymbols;
#else //PQ_GENCODE
	// Actual values are written by gencode to hashParams.acpp
	static const int hashTableSize_UnitSymbols;
	static const int hashTableSize_UnitLongNames;
	static const int hashTableSize_PrefixSymbols;
	static const int hashTableSize_UnitPlurals;
	static const int hashTableSeed_UnitSymbols;
	static const int hashTableSeed_UnitLongNames;
	static const int hashTableSeed_UnitPlurals;
	static const int hashTableSeed_PrefixSymbols;
#endif //PQ_GENCODE

	//==================================================================================
	// Hashing function 
	//    originally to be used as a template parameter to unordered_map,
	//    but that wastes a lot of RAM
	static const size_t defaultHashSeed;
	struct cstrHasherTiny
	{
	public:
		cstrHasherTiny();
		cstrHasherTiny(size_t seed_p);
		size_t operator()(const CSubString& s) const;
		size_t seed;
#ifdef NO_INLINE
		size_t operator()(const char* s) const;
#else
		INLINE_KEYWORD size_t operator()(const char* s) const { return operator()(CSubString(s)); }
#endif

	};

#endif //#ifndef NO_HASHING
#endif //#if !defined(NO_TEXT) || defined(PQ_GENCODE)

	// End hashing function
	//==================================================================================


	//==================================================================================
	// Error handling stuff
#ifdef NO_THROW
	enum ErrorCode
	{
		E_SUCCESS = 0,
		E_UNIT_MISMATCH,
		E_INVALID_EXPRESSION,
		E_MEMORY,
		E_LOGIC_ERROR,
		E_HEADER_CONFIG,
		E_OVERFLOW
	};
	static void (*errorHandler)(void* userContext, ErrorCode e);
	static void* errorUserContext;
#ifndef NO_INLINE
	INLINE_KEYWORD static void SetErrorHandler(void (*errorHandler_arg)(void* context, ErrorCode e)) { errorHandler = errorHandler_arg; }
#else
	static void SetErrorHandler(void (*errorHandler_arg)(void* context, ErrorCode e));
#endif
#else
	class UnitMismatchException : public std::runtime_error
	{
	public:
		UnitMismatchException();
		UnitMismatchException(const char* message);
	};
	class InvalidExpressionException : public std::runtime_error
	{
	public:
		InvalidExpressionException();
		InvalidExpressionException(const char* message);
	};
	class HeaderConfigException : public std::runtime_error
	{
	public:
		HeaderConfigException();
		HeaderConfigException(const char* message);
	};
#endif  // !NO_THROW
	// End error handling stuff
	//==================================================================================


	//==================================================================================
	// Formatting stuff 
	typedef unsigned short unitIndex_t;
	typedef signed char prefixIndex_t;
	typedef signed char unitPower_t;
#ifndef NO_TEXT
	class UnitListBase
	{
		friend class PhysicalQuantity;
	public:
		struct UnitPref
		{
			unitIndex_t iUnit;
			prefixIndex_t iPrefix;
			unitPower_t power;
			bool anyPrefix;
		};
		~UnitListBase(); // Note: This was virtual, but decided against it. These are not
			// allocated from within the library.
			// Any user instantiating a UnitList should know (by typedef) whether it's static or dynamic.
			// Having it virtual caused problems on Arduino with undefined linker referenced to deleting dtor.
	protected:
		UnitPref* unitIndeces;
		int count_;
		const UnitPref& operator[] (int i) const;
		void build(const CSubString& unitList, UnitPref* buffer, int bufferSizeBytes, bool dynamic);
#ifdef NO_INLINE
		int count() const;
#else
		INLINE_KEYWORD int count() const { return count_; }
#endif
	private:
		bool dynamic;
	};

#ifndef NO_NEW
	class UnitList_dynamic : public UnitListBase
	{
	public:
		UnitList_dynamic(const CSubString& unitList_SpaceDelimited);
		~UnitList_dynamic();
	};
#endif //#ifndef NO_NEW
	class UnitList_static : public UnitListBase
	{
	public:
		UnitList_static(const CSubString& unitList_SpaceDelimited, void* storage, size_t storageSizeBytes);
		UnitList_static(const CSubString& unitList_SpaceDelimited);
		~UnitList_static() {}
	};
#ifdef NO_NEW
	typedef UnitList_static UnitList;
#else
	typedef UnitList_dynamic UnitList;
#endif
#endif //#ifndef NO_TEXT

	// End formatting stuff
	//==================================================================================



	//==================================================================================
	// Main class members


	// Constructors
	void init();
#ifndef NO_TEXT
	PhysicalQuantity(num value, const char* unit);
	PhysicalQuantity(CSubString str);
#endif // #ifndef NO_TEXT
#if defined(YES_CONSTEXPR)
	constexpr PhysicalQuantity()
	 : value{0}, dim{0,0,0,0,0}, iofs(0) {}
	constexpr PhysicalQuantity(const PhysicalQuantity& copy)
	 : value{copy.value}, dim{copy.dim[0], copy.dim[1], copy.dim[2], copy.dim[3], copy.dim[4]}, iofs(copy.iofs) {}
	constexpr PhysicalQuantity(num value_p, const signed char dim_p[(int)QuantityType::ENUM_MAX])
	 : value(value_p), dim {dim_p[0], dim_p[1], dim_p[2], dim_p[3], dim_p[4]}, iofs(0) {}
	constexpr PhysicalQuantity(num value_p)
	 : value{value_p}, dim{0,0,0,0,0}, iofs(0) {}
	constexpr PhysicalQuantity(int value_p)
	 : value{(num)value_p}, dim{0,0,0,0,0}, iofs(0) {}
#ifdef _CHRONO_
	template <typename numType, intmax_t numer, intmax_t denom>
	constexpr PhysicalQuantity(std::chrono::duration<numType, std::ratio<numer, denom>> t)
	  : value {t.count() * (num)numer / (num)denom},
	   dim{0,0,1,0,0}, iofs(0) {}
	
	constexpr num offset() const { return rom(KnownUnitOffsets[iofs]); }
	constexpr num is(const PhysicalQuantity& b) const { return dim == b.dim; }
	constexpr num extract(const PhysicalQuantity& b) const
	{
		if (dim != b.dim) { throw UnitMismatchException("PQ::as() conversion to dissimilar quantity"); }
		return (value + offset()) / b.value - b.offset();
	}

	//: value {std::chrono::duration<num, std::ratio<1,PQ_CHRONO_PRECISION>>(t).count() * (num)PQ_CHRONO_PRECISION },
	//: value {std::chrono::duration<num, std::ratio<1,1>>(t).count() * (num)numer / (num)denom},
   //template<typename numType, intmax_t numer, intmax_t denom>
	//constexpr PhysicalQuantity(std::chrono::duration<int, std::ratio<numer, denom>> t)
	// : value {t.count() * (num)numer / (num)denom },
	//   dim{0,0,0,0,0} {}
#endif // chrono
#else // constexpr 
#ifdef NO_INLINE
	PhysicalQuantity();
	PhysicalQuantity(const PhysicalQuantity& copy);
	PhysicalQuantity(num value_p, const signed char dim_p[(int)QuantityType::ENUM_MAX]);
	PhysicalQuantity(num value);
	PhysicalQuantity(int value);
#ifdef _CHRONO_
	template <typename numType, intmax_t numer, intmax_t denom>
	PhysicalQuantity(std::chrono::duration<numType, std::ratio<numer, denom>> t)
		: value{ t.count() * (num)numer / (num)denom },
		dim{ 0,0,1,0,0 } {}
#endif // chrono
#else // inline
	INLINE_KEYWORD PhysicalQuantity() { init(); }
	INLINE_KEYWORD PhysicalQuantity(const PhysicalQuantity& copy) { value=copy.value; memcpy(dim, copy.dim, sizeof(dim)); iofs = copy.iofs; }
	INLINE_KEYWORD PhysicalQuantity(num value_p, const signed char dim_p[(int)QuantityType::ENUM_MAX])
	{
		value = value_p;
		memcpy(dim, dim_p, sizeof(dim));
		iofs = 0;
	}
	INLINE_KEYWORD PhysicalQuantity(num value_p) { init(); value = value_p; }
	INLINE_KEYWORD PhysicalQuantity(int value_p) { init(); value = (num)value_p; }
#ifdef _CHRONO_
	template <typename numType, intmax_t numer, intmax_t denom>
	INLINE_KEYWORD PhysicalQuantity(std::chrono::duration<numType, std::ratio<numer, denom>> t)
		: value{ t.count() * (num)numer / (num)denom },
		dim{ 0,0,1,0,0 }, iofs(0) {}
#endif // chrono
#endif // NO_INLINE

#endif // constexpr

	PhysicalQuantity& operator=(num value);
	PhysicalQuantity& operator=(const PhysicalQuantity& cp);
	~PhysicalQuantity() = default;


enum sprintFlags
{
	SPRINT_SLASH = 1,
	SPRINT_NO_SLASH = 0,
	SPRINT_LONG_NAMES = 2,
	SPRINT_SYMBOLS = 0
};

#ifndef NO_TEXT
	void parse(const CSubString& text);
	num convert(const CSubString& units) const;

	// sprint() has many overloads, but this is the main one they all call eventually
	size_t sprint(char* buf, size_t bufsize, unsigned int precision, const UnitListBase& pu, int flags = 0) const;

#ifdef NO_INLINE
	size_t sprint(char* buf, size_t bufsize, unsigned int precision, int flags = 0) const;
	size_t sprint(char* buf, size_t bufsize, unsigned int precision, const CSubString& sspu, void* puBuf, size_t puBufSize, int flags = 0) const;
	size_t sprint(char* buf, size_t bufsize, unsigned int precision, const CSubString& sspu, int flags = 0) const;
#else //#ifdef NO_INLINE
	size_t sprint(char* buf, size_t bufsize, unsigned int precision, int flags = 0) const
		{ return sprint(buf,bufsize, precision, UnitList(""), flags); }
	size_t sprint(char* buf, size_t bufsize, unsigned int precision, const CSubString& sspu, void* puBuf, size_t puBufSize, int flags = 0) const
		{ return sprint(buf, bufsize, precision, UnitList_static(sspu, puBuf, puBufSize), flags); }
	size_t sprint(char* buf, size_t bufsize, unsigned int precision, const CSubString& sspu, int flags = 0) const
		{ return sprint(buf, bufsize, precision, UnitList(sspu), flags); }
#endif //#ifdef NO_INLINE

	static size_t printNum(char* buf, size_t size, num value, unsigned int precision);
//#ifndef NO_INLINE
//	INLINE_KEYWORD size_t sprint(char* buf, size_t size, unsigned int precision, bool useSlash = true) const
//	{
//		UnitList u("");
//		return sprint(buf, size, precision, u, useSlash);
//	}
//#else // #ifndef NO_INLINE
//	size_t sprint(char* buf, size_t size, const char* pu, bool useSlash = true) const;
//	size_t sprint(char* buf, size_t size, unsigned int precision, bool useSlash = true) const;
//#endif // #ifndef NO_INLINE
#endif //#ifndef NO_TEXT

#ifdef NO_INLINE
	static PhysicalQuantity get1kg();
	static PhysicalQuantity get1m();
	static PhysicalQuantity get1s();
	static PhysicalQuantity get1K();
	static PhysicalQuantity get1A();
#ifndef NO_TEXT
	static PhysicalQuantity fromUnit(const UnitDefinition& u);
#endif
#else // NO_INLINE
	static PhysicalQuantity get1kg() { const signed char d[5]={1,0,0,0,0}; return PhysicalQuantity(1.0, d); }
	static PhysicalQuantity get1m() { const signed char d[5]={0,1,0,0,0}; return PhysicalQuantity(1.0, d); }
	static PhysicalQuantity get1s() { const signed char d[5]={0,0,1,0,0}; return PhysicalQuantity(1.0, d); }
	static PhysicalQuantity get1K() { const signed char d[5]={0,0,0,1,0}; return PhysicalQuantity(1.0, d); }
	static PhysicalQuantity get1A() { const signed char d[5]={0,0,0,0,1}; return PhysicalQuantity(1.0, d); }
#ifndef NO_TEXT
	PhysicalQuantity fromUnit(const UnitDefinition& u) { PhysicalQuantity ret(u.factor, u.dim); return ret; }
#endif
#endif // #else of #ifdef NO_INLINE

#ifndef NO_TEXT
	static PhysicalQuantity fromUnit(const UnitDefinition& u, int power = 1);
#endif

	bool isScalar();
	num getScalar(); // Error if the value has units

#ifndef NO_TEXT
#ifndef NO_STD_STRING
	std::string toString(int flags = 0) const;
	std::string toString(const UnitListBase& preferredUnits, int flags = 0) const;
	std::string toString(const CSubString& preferredUnits, int flags = 0) const;
#endif // #ifndef NO_STD_STRING
#endif //#ifndef NO_TEXT

// Time conversion - in case people need their literals back
#ifdef _CHRONO_
#ifdef NO_CONSTEXPR
	operator std::chrono::duration<num, std::ratio<1, 1>>();
#else // constexpr
	constexpr operator std::chrono::duration<num, std::ratio<1, 1>>()
	{
		if ((dim[(int)QuantityType::MASS] != 0)
			|| (dim[(int)QuantityType::DISTANCE] != 0)
			|| (dim[(int)QuantityType::TIME] != 1)
			|| (dim[(int)QuantityType::TEMPERATURE] != 0)
			|| (dim[(int)QuantityType::CURRENT] != 0))
		{
#ifdef NO_THROW
			errorHandler(errorUserContext, E_UNIT_MISMATCH);
#else
			throw UnitMismatchException("Not a time value.");
#endif
			return std::chrono::duration<num, std::ratio<1, 1>>(0);
		}
		else { return std::chrono::duration<num, std::ratio<1, 1>>(value); }
	}
#endif // constexpr
#endif // chrono


#if defined(YES_CONSTEXPR)
	constexpr PhysicalQuantity operator* (const PhysicalQuantity& rhs) const 
	{
		signed char d[(int)QuantityType::ENUM_MAX]
		{
			(signed char)(dim[0] + rhs.dim[0]),  
			(signed char)(dim[1] + rhs.dim[1]),  
			(signed char)(dim[2] + rhs.dim[2]),  
			(signed char)(dim[3] + rhs.dim[3]),  
			(signed char)(dim[4] + rhs.dim[4])
		};
		//return PhysicalQuantity(value * rhs.value, d);
		if (iofs == 0 && rhs.iofs == 0) { return PhysicalQuantity(value * rhs.value, d); }
		return PhysicalQuantity((value - rom(KnownUnitOffsets[iofs])) * (rhs.value - rom(KnownUnitOffsets[rhs.iofs])), d);
	}
	constexpr PhysicalQuantity operator/ (const PhysicalQuantity& rhs) const 
	{
		signed char d[(int)QuantityType::ENUM_MAX]
		{
			(signed char)(dim[0] - rhs.dim[0]),  
			(signed char)(dim[1] - rhs.dim[1]),  
			(signed char)(dim[2] - rhs.dim[2]),  
			(signed char)(dim[3] - rhs.dim[3]),  
			(signed char)(dim[4] - rhs.dim[4])
		};
		//return PhysicalQuantity(value / rhs.value, d);
		if (iofs == 0 && rhs.iofs == 0) { return PhysicalQuantity(value / rhs.value, d); }
		return PhysicalQuantity((value - rom(KnownUnitOffsets[iofs])) / (rhs.value - rom(KnownUnitOffsets[rhs.iofs])), d);
	}
#else
//#endif //#if defined(YES_CONSTEXPR)
	PhysicalQuantity operator* (const PhysicalQuantity& rhs) const;
	PhysicalQuantity operator/ (const PhysicalQuantity& rhs) const;
#endif //#if defined(YES_CONSTEXPR)

	PhysicalQuantity operator+ (const PhysicalQuantity& rhs) const;
	PhysicalQuantity operator- (const PhysicalQuantity& rhs) const;
	PhysicalQuantity& operator++(); // Will throw an error if not scalar
	PhysicalQuantity& operator--();
	PhysicalQuantity pow(int exp_numerator, int exp_denominator = 1);

#if defined(YES_CONSTEXPR)
	constexpr PhysicalQuantity operator* (num rhs)
	{
		signed char d[(int)QuantityType::ENUM_MAX]
		{
			dim[0],
			dim[1],  
			dim[2],  
			dim[3],  
			dim[4]
		};
		return PhysicalQuantity(value * rhs, d);
	}
	constexpr PhysicalQuantity operator/ (num rhs)
	{
		signed char d[(int)QuantityType::ENUM_MAX]
		{
			dim[0],
			dim[1],  
			dim[2],  
			dim[3],  
			dim[4]
		};
		return PhysicalQuantity(value * rhs, d);
	}
#else //#if defined(YES_CONSTEXPR)
	PhysicalQuantity operator* (num rhs) const;
	PhysicalQuantity operator/ (num rhs) const;
#endif //#if defined(YES_CONSTEXPR)
	PhysicalQuantity operator+ (num rhs) const;
	PhysicalQuantity operator- (num rhs) const;

	PhysicalQuantity& operator *= (const PhysicalQuantity& rhs) { *this = (*this) * rhs; return *this; }
	PhysicalQuantity& operator /= (const PhysicalQuantity& rhs) { *this = (*this) / rhs; return *this; }
	PhysicalQuantity& operator += (const PhysicalQuantity& rhs) { *this = (*this) + rhs; return *this; }
	PhysicalQuantity& operator -= (const PhysicalQuantity& rhs) { *this = (*this) - rhs; return *this; }
	PhysicalQuantity& operator *= (const num& rhs) { *this = (*this) * rhs; return *this; }
	PhysicalQuantity& operator /= (const num& rhs) { *this = (*this) / rhs; return *this; }
	PhysicalQuantity& operator += (const num& rhs) { *this = (*this) + rhs; return *this; }
	PhysicalQuantity& operator -= (const num& rhs) { *this = (*this) - rhs; return *this; }


	bool operator==(const PhysicalQuantity& rhs) const;
	bool operator>=(const PhysicalQuantity& rhs) const;
	bool operator<=(const PhysicalQuantity& rhs) const;
	bool operator>(const PhysicalQuantity& rhs) const;
	bool operator<(const PhysicalQuantity& rhs) const;
	bool operator!=(const PhysicalQuantity& rhs) const;

	bool like(const PhysicalQuantity& rhs) const; // Same kind of quantity.

#ifndef NO_NETWORK
	bool writeNetworkBinary(void* buf);
	bool readNetworkBinary(void* buf);
#endif

#if !defined(NO_TEXT) || defined(PQ_GENCODE)
	//static const UnitDefinition KnownUnits[];
	static DECLARE_CONST_ARRAY(UnitDefinition, KnownUnits);
	static const unitIndex_t KnownUnitsLength;
	//static const num KnownUnitOffsets[];
	static DECLARE_CONST_ARRAY(num, KnownUnitOffsets);
	static const unitIndex_t KnownUnitOffsetsLength;

	//static const Prefix KnownPrefixes[];
	static DECLARE_CONST_ARRAY(Prefix, KnownPrefixes);

	static const prefixIndex_t KnownPrefixesLength;
	static const prefixIndex_t dekaIndex;

	// Because the standard unit is the kilogram...
	static const prefixIndex_t kiloIndex;
	static const unitIndex_t gramIndex;

#ifndef NO_TEXT
	static PhysicalQuantity eval(CSubString str, const void* vars_internal = nullptr);
#ifdef PQVARS
	static PhysicalQuantity eval(CSubString str, const PQVarList& vars) { return eval(str, &vars); }
#endif
	//static PhysicalQuantity eval1(CSubString str);
	static bool findUnit(CSubString name, unitIndex_t& outUnitIndex, prefixIndex_t& outPrefixIndex);
#endif
#endif //#if !defined(NO_TEXT) || defined(PQ_GENCODE)

	static const int compiledHeaderOptions;

	static num equalityToleranceFactor;
	static bool feq(num a, num b, num toleranceFactor);


private:
	num value;
	signed char dim[(int)QuantityType::ENUM_MAX];
	uint8_t iofs = 0;

	unsigned int magdim() const; // Magnitude of dimension = sum(abs(dim[x]))

#ifndef NO_TEXT
	static void parseUnits(const CSubString& unitStr, signed char (&dimOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut, uint8_t& iofsOut); // throws if unknown/invalid unit
	static void mulUnit(signed char (&dimOut)[(int)QuantityType::ENUM_MAX], const UnitDefinition& unit, signed int power, bool invert = false); // deals only with quantity dimension, conversion factors are handled elsewhere
	int bestReductionPower(const UnitDefinition& unit, bool preferred = false) const;  // Divide by what power of (unit) to minimize magdim? Used in text output logic.
	void WriteOutputUnit(int plen, int ulen, int reduceExp, size_t &outofs, size_t size, char * buf, prefixIndex_t ipre, const PhysicalQuantity::UnitDefinition & testunit, bool longNames = false) const;
	void WriteOutputUnit(const PhysicalQuantity::UnitDefinition& unit, int power, prefixIndex_t iPrefix, char* buf, size_t size, size_t cursorPos, size_t &out_cursorPosAfter, bool longNames = false) const;
#endif


public:
#ifdef NO_INLINE
	PhysicalQuantity(const char* str);

	bool getDim(signed char *d, int bufSize);

#ifndef NO_TEXT
	num convert(const char* units) const;
	static bool findUnit(const char* pcharName, unitIndex_t& outUnitIndex, prefixIndex_t& outPrefixIndex);
	void parse(const char* text);
#endif //#ifndef NO_TEXT
#else //#ifdef NO_INLINE

	bool getDim(signed char* d, int bufSize)
	{
		if (bufSize < (int)QuantityType::ENUM_MAX) { return false; }
		memcpy(d, dim, (int)QuantityType::ENUM_MAX);
		return true;
	}

	constexpr std::array<signed char, (int)QuantityType::ENUM_MAX> Dim() const
	{
		static_assert((int)QuantityType::ENUM_MAX == 5, "Dimension count changed");
		return std::array<signed char,(int)QuantityType::ENUM_MAX>{dim[0], dim[1], dim[2], dim[3], dim[4]};
	}


#ifndef NO_TEXT
	// TODO: Maybe one day make this constexpr and inline
	INLINE_KEYWORD PhysicalQuantity(const char* str) { PhysicalQuantity(CSubString(str)); }
	INLINE_KEYWORD num convert(const char* units) const { return convert(CSubString(units)); }
	INLINE_KEYWORD static bool findUnit(const char* pcharName, unitIndex_t& outUnitIndex, prefixIndex_t& outPrefixIndex) { return findUnit(CSubString(pcharName), outUnitIndex, outPrefixIndex); }
	INLINE_KEYWORD void parse(const char* text) { parse(CSubString(text)); }

#endif //#ifndef NO_TEXT
#endif //#else of #ifdef NO_INLINE

	static bool findRatio(num test, std::pair<int, int>& out);

	// End main class members
	//==================================================================================
};
// END class PhysicalQuantity
//===================================================================================================


#ifndef NO_TYPEDEFS
typedef PhysicalQuantity PQ;
#ifndef NO_TEXT
typedef PhysicalQuantity::CSubString csubstr;
#endif // #ifndef NO_TEXT
#endif // #ifndef NO_TYPEDEFS



// Static Physical Quantity
template <signed char DMa, signed char DDi,signed char DTi,signed char DTe,signed char DCu, unsigned char iofs>
class SPQ
{
public:
	template <signed char BMa, signed char BDi,signed char BTi,signed char BTe,signed char BCu, unsigned char Biofs>
	friend class SPQ; //<BMa, BDi, BTi, BTe, BCu, Biofs>;

	using num = PQ::num;
	static constexpr int NDIM = PQ::NDIM;
	constexpr std::array<signed char, NDIM> Dim() const { return {DMa,DDi,DTi,DTe,DCu}; }

private:
	PQ::num value = 0;
	//uint8_t iofs = 0;
	static constexpr signed char dim[NDIM] = { DMa, DDi, DTi, DTe, DCu };

public:
	constexpr SPQ() : value{0} {} //, iofs{0} {}
	constexpr SPQ(PQ::num v) : value{v} //, iofs{0}
	{
		//static_assert(NDIM == 5, "Dimension count changed");
		//static_assert(dim[0] == 0 && dim[1] == 0 && dim[2] == 0 && dim[3] == 0 && dim[4] == 0, "Quantity mismatch (scalar)");
		value = v;
	}
	constexpr SPQ(const SPQ<DMa,DDi,DTi,DTe,DCu,iofs>& b)
		: value {b.value} //, iofs {b.iofs}
	{
		//static_assert(PQ::NDIM == 5);
		//static_assert(dim[0] == b.dim[0] && dim[1] == b.dim[1] && dim[2] == b.dim[2] && dim[3] == b.dim[3] && dim[4] == b.dim[4], "Quantity mismatch (scalar)");
	}
	constexpr SPQ& operator = (const SPQ<DMa,DDi,DTi,DTe,DCu,iofs>& b)
	{
		//static_assert(PQ::NDIM == 5);
		//static_assert(dim[0] == b.dim[0] && dim[1] == b.dim[1] && dim[2] == b.dim[2] && dim[3] == b.dim[3] && dim[4] == b.dim[4], "Quantity mismatch (scalar)");
		value = b.value;
		//iofs = b.iofs;
		return *this;
	}
	constexpr SPQ& operator = (const PhysicalQuantity::num b)
	{
		static_assert(dim[0] == 0 && dim[1] == 0 && dim[2] == 0 && dim[3] == 0 && dim[4] == 0, "Quantity mismatch (scalar)");
		value = b;
	}
	~SPQ() = default;

	constexpr SPQ(const PQ& p)
		: value { p.value } //, iofs {p.iofs}
	{
		static_assert((int)(PhysicalQuantity::QuantityType::ENUM_MAX) == 5, "Number of dimensions has changed");
		static_assert(p.Dim()[0] == dim[0]);
		static_assert(p.Dim()[1] == dim[1]);
		static_assert(p.Dim()[2] == dim[2]);
		static_assert(p.Dim()[3] == dim[3]);
		static_assert(p.Dim()[4] == dim[4]);
		static_assert(p.iofs == iofs);
		//if (
		//	p.Dim()[0] != dim[0] ||
		//	p.Dim()[1] != dim[1] ||
		//	p.Dim()[2] != dim[2] ||
		//	p.Dim()[3] != dim[3] ||
		//	p.Dim()[4] != dim[4]
		//	)
		//{
		//	throw std::bad_cast();
		//}
	}

	SPQ& operator += (const SPQ& b)
	{
		static_assert(dim == b.dim, "Quantity mismatch");
		if (b.iofs == 0) { value += b.value; }
		else { value += b.value - rom(PQ::KnownUnitOffsets[b.iofs]); }
		return *this;
	}
	SPQ& operator -= (const SPQ& b)
	{
		static_assert(dim == b.dim, "Quantity mismatch");
		if (b.iofs == 0) { value += b.value; }
		else { value -= b.value - rom(PQ::KnownUnitOffsets[b.iofs]); }
		return *this;
	}
	SPQ& operator *= (const SPQ& b)
	{
		static_assert(NDIM == 5, "Dimension count changed");
		static_assert(b.dim[0] == 0 && b.dim[1] == 0 && b.dim[2] == 0 && b.dim[3] == 0 && b.dim[4] == 0, "New quantity can not be assigned to this var");
		value = (value - rom(PQ::KnownUnitOffsets[iofs])) * b.value + rom(PQ::KnownUnitOffsets[iofs]);
		//iofs = 0;
		return *this;
	}
	SPQ& operator /= (const SPQ& b)
	{
		static_assert(NDIM == 5, "Dimension count changed");
		static_assert(b.dim[0] == 0 && b.dim[1] == 0 && b.dim[2] == 0 && b.dim[3] == 0 && b.dim[4] == 0, "New quantity can not be assigned to this var");
		value = (value - rom(PQ::KnownUnitOffsets[iofs])) / b.value + rom(PQ::KnownUnitOffsets[iofs]);
		//iofs = 0;
		return *this;
	}

	SPQ& operator *= (PQ::num bnum)
	{
		value = (value - rom(PQ::KnownUnitOffsets[iofs])) * bnum + rom(PQ::KnownUnitOffsets[iofs]);
		return *this;
	}
	SPQ& operator /= (PQ::num bnum)
	{
		value = (value - rom(PQ::KnownUnitOffsets[iofs])) / bnum + rom(PQ::KnownUnitOffsets[iofs]);
		return *this;
	}

	SPQ operator + (const SPQ& b)
	{
		static_assert(dim == b.dim, "Quantity mismatch");
		SPQ r = *this;
		r += b;
		return r;
	}
	SPQ operator - (const SPQ& b)
	{
		static_assert(dim == b.dim, "Quantity mismatch");
		SPQ r = *this;
		r -= b;
		return r;
	}

	template <signed char BMa, signed char BDi, signed char BTi, signed char BTe, signed char BCu, unsigned char Biofs>
	auto operator * (const SPQ<BMa,BDi,BTi,BTe,BCu, Biofs>& b)
	-> SPQ<DMa+BMa, DDi+BDi, DTi+BTi, DTe+BTe, DCu+BCu, 0>
	{
		//SPQ<stype1() * b.stype1()> r;
		SPQ<DMa+BMa, DDi+BDi, DTi+BTi, DTe+BTe, DCu+BCu, 0> r;
		static_assert(NDIM == 5, "Dimension count changed");
		//if (iofs) { value -= rom(PQ::KnownUnitOffsets[iofs]); }
		r.value = (value - rom(PQ::KnownUnitOffsets[iofs])) * (b.value - rom(PQ::KnownUnitOffsets[Biofs]));
		//r.iofs = 0;
		return r;
	}

	template <signed char BMa, signed char BDi, signed char BTi, signed char BTe, signed char BCu, unsigned char Biofs>
	auto operator / (const SPQ<BMa,BDi,BTi,BTe,BCu, Biofs>& b)
	-> SPQ<DMa-BMa, DDi-BDi, DTi-BTi, DTe-BTe, DCu-BCu, 0>
	{
		SPQ<DMa-BMa, DDi-BDi, DTi-BTi, DTe-BTe, DCu-BCu, 0> r;
		static_assert(NDIM == 5, "Dimension count changed");
		//r.value = value / b.value;
		//r.iofs = 0;
		r.value = (value - rom(PQ::KnownUnitOffsets[iofs])) * (b.value - rom(PQ::KnownUnitOffsets[Biofs]));
		return r;
	}

	template <int P>
	auto pow() -> SPQ<DMa*P, DDi*P, DTi*P, DTe*P, DCu*P, 0>
	{
		SPQ<DMa*P, DDi*P, DTi*P, DTe*P, DCu*P, 0> r;
		static_assert(NDIM == 5, "Dimension count changed");
		//r.value = pow(value - , P);
		//r.iofs = 0;
		r.value = pow(value - rom(PQ::KnownUnitOffsets[iofs]), P);
	}

	PQ pq()
	{
		PQ r;
		r.value = value;
		r.iofs = iofs;
		for (int i = 0; i < NDIM; i++) { r.dim[i] = dim[i]; }
		return r;
	}

	operator PQ ()
	{
		return pq();
	}

	constexpr PQ::num offset() const { return rom(PQ::KnownUnitOffsets[iofs]); }

	constexpr PQ::num extract(const SPQ& b) const
	{
		static_assert(dim == b.dim);
		return (value - offset()) / b.value + b.offset();
	}


	constexpr PQ::num extract(const PQ& b) const
	{
		PQ me = PQ(*this);
		if (me.dim != b.dim) { throw PQ::UnitMismatchException("SPQ::as(): conversion to dissimilar quantity"); }
		return (value - offset()) / b.value + b.offset();
	}

	constexpr bool is(const SPQ& b) const
	{
		return dim == b.dim;
	}

	constexpr bool is(const PQ& b) const
	{
		return dim == b.dim;
	}

};

//template <const PQ& Q>
//using PQS = SPQ<Q.Dim()[0], Q.Dim()[1], Q.Dim()[2], Q.Dim()[3], Q.Dim()[4], Q.iofs>;


#ifdef NO_INLINE
PhysicalQuantity operator*(PhysicalQuantity::num left, const PhysicalQuantity& right);
PhysicalQuantity operator/(PhysicalQuantity::num left, const PhysicalQuantity& right);
PhysicalQuantity operator+(PhysicalQuantity::num left, const PhysicalQuantity& right);
PhysicalQuantity operator-(PhysicalQuantity::num left, const PhysicalQuantity& right);
#else
INLINE_KEYWORD PhysicalQuantity operator*(PhysicalQuantity::num left, const PhysicalQuantity& right) { return PhysicalQuantity(left) * right; }
INLINE_KEYWORD PhysicalQuantity operator/(PhysicalQuantity::num left, const PhysicalQuantity& right) { return PhysicalQuantity(left) / right; }
INLINE_KEYWORD PhysicalQuantity operator+(PhysicalQuantity::num left, const PhysicalQuantity& right) { return PhysicalQuantity(left) + right; }
INLINE_KEYWORD PhysicalQuantity operator-(PhysicalQuantity::num left, const PhysicalQuantity& right) { return PhysicalQuantity(left) - right; }
#endif

// Now that the PQ class is complete...

//==================================================================================
// Variables
#ifdef PQVARS
//#if !defined(NO_NEW)

class PQVarList : public std::unordered_map<std::string, PhysicalQuantity>
{
public:
	bool set(const PhysicalQuantity::CSubString& name, PhysicalQuantity value);
	PQ get(const PhysicalQuantity::CSubString name) const;
	bool contains(const PQ::CSubString name) const;
	// erase() already exists in unordered_map
	static bool isLegalName(PQ::CSubString name);
};

#else  // #ifdef PQVARS
//class PQVarList
//{
//public:
//	bool set(const PhysicalQuantity::CSubString& name, PhysicalQuantity value) { return false; }
//	PQ get(const PhysicalQuantity::CSubString name) const { return PQ(NAN); }
//	int erase(const PhysicalQuantity::CSubString name) { return 0; }
//	bool contains(const PQ::CSubString name) const { return false; }
//	static bool isLegalName(PQ::CSubString name) { return false; }
//};
#endif // #ifdef PQVARS

// End variables
//==================================================================================


#define PQHeaderOptionsMatch (PQ_HEADER_OPTIONS == PhysicalQuantity::compiledHeaderOptions)


//==================================================================================
// Literals
// Use the Declare... macros in a header, Define... in a source file.
// gencode should take care of all that automatically.

#ifdef NO_LITERALS
#define DeclareLiteral(symbol_no_quotes)
#else

#define DeclareLiteral(symbol_no_quotes) PhysicalQuantity operator ""_##symbol_no_quotes(long double); PhysicalQuantity operator ""_##symbol_no_quotes(unsigned long long);


/********************************************************************************
// gencode now makes all the individual prefix literals based on flags
#define DeclareLiteralPrefixes(symbol_no_quotes) \
	DeclareLiteral( c##symbol_no_quotes) \
	DeclareLiteral( k##symbol_no_quotes) \
	DeclareLiteral( m##symbol_no_quotes) \
	DeclareLiteral( M##symbol_no_quotes) \
	DeclareLiteral( u##symbol_no_quotes) \
	DeclareLiteral( G##symbol_no_quotes) \
	DeclareLiteral( n##symbol_no_quotes) \
	DeclareLiteral( T##symbol_no_quotes) \
	DeclareLiteral( p##symbol_no_quotes) \
	DeclareLiteral( P##symbol_no_quotes) \
	DeclareLiteral(da##symbol_no_quotes) \
	DeclareLiteral( f##symbol_no_quotes) \
	DeclareLiteral( d##symbol_no_quotes) \
	DeclareLiteral( h##symbol_no_quotes) \
	DeclareLiteral( E##symbol_no_quotes) \
	DeclareLiteral( a##symbol_no_quotes) \
	DeclareLiteral( z##symbol_no_quotes) \
	DeclareLiteral( Z##symbol_no_quotes) \
	DeclareLiteral( y##symbol_no_quotes) \
	DeclareLiteral( Y##symbol_no_quotes)

#define DeclareLiteralWithPrefixes(symbol_no_quotes)  \
	DeclareLiteral(symbol_no_quotes) \
	DeclareLiteralPrefixes(symbol_no_quotes)
// ***************************************************************************************/


// Use these in a compiled .cpp file
#define DefineLiteralBase(symbol_no_quotes, factor) \
	PhysicalQuantity operator ""_##symbol_no_quotes (long double v) \
	{ \
		return PhysicalQuantity((PhysicalQuantity::num)v * factor, #symbol_no_quotes); \
	} \
	PhysicalQuantity operator ""_##symbol_no_quotes (unsigned long long v) \
	{ \
		return PhysicalQuantity((PhysicalQuantity::num)(v * factor), #symbol_no_quotes); \
	}
#define DefineLiteral(symbol_no_quotes) DefineLiteralBase(##symbol_no_quotes, 1.0)
#define DefineLiteralWithPrefixes(symbol_no_quotes) \
	DefineLiteralBase(  symbol_no_quotes, 1.0   ) \
	DefineLiteralBase( c##symbol_no_quotes, 1e-2  ) \
	DefineLiteralBase( k##symbol_no_quotes, 1e3   ) \
	DefineLiteralBase( m##symbol_no_quotes, 1e-3  ) \
	DefineLiteralBase( M##symbol_no_quotes, 1e6   ) \
	DefineLiteralBase( u##symbol_no_quotes, 1e-6  ) \
	DefineLiteralBase( G##symbol_no_quotes, 1e9   ) \
	DefineLiteralBase( n##symbol_no_quotes, 1e-9  ) \
	DefineLiteralBase( T##symbol_no_quotes, 1e12  ) \
	DefineLiteralBase( p##symbol_no_quotes, 1e-12 ) \
	DefineLiteralBase( P##symbol_no_quotes, 1e15  ) \
	DefineLiteralBase(da##symbol_no_quotes, 10.0  ) \
	DefineLiteralBase( f##symbol_no_quotes, 1e-15 ) \
	DefineLiteralBase( d##symbol_no_quotes, 0.1   ) \
	DefineLiteralBase( h##symbol_no_quotes, 100.0 ) \
	DefineLiteralBase( E##symbol_no_quotes, 1e18  ) \
	DefineLiteralBase( a##symbol_no_quotes, 1e-18 ) \
	DefineLiteralBase( z##symbol_no_quotes, 1e-21 ) \
	DefineLiteralBase( Z##symbol_no_quotes, 1e21  ) \
	DefineLiteralBase( y##symbol_no_quotes, 1e-24 ) \
	DefineLiteralBase( Y##symbol_no_quotes, 1e24  ) \


#if defined(YES_CONSTEXPR)
// Use these in a header
// TODO: Change these macros to iofs rather than value of ofs
#define CxLiteral(symbol_no_quotes, Ma, Di, Ti, Te, Cu, factor, iofs) \
	static constexpr auto operator ""_##symbol_no_quotes(long double v) \
	{ \
		return SPQ<Ma,Di,Ti,Te,Cu, iofs>(v * factor); \
	} \
	static constexpr auto operator ""_##symbol_no_quotes(unsigned long long v) \
	{ \
		return SPQ<Ma,Di,Ti,Te,Cu, iofs>(v * factor); \
	}
    /*#pragma message("Compiling literal symbol " #symbol_no_quotes) */ \
//#define CxLiteral(symbol_no_quotes, Ma, Di, Ti, Te, Cu, factor, offset) \
//	static constexpr auto operator ""_##symbol_no_quotes(long double v) \
//{ \
//	signed char d[(int)PhysicalQuantity::QuantityType::ENUM_MAX]  \
//{Ma, Di, Ti, Te, Cu}; \
//	auto r = PhysicalQuantity(((PhysicalQuantity::num)factor * (PhysicalQuantity::num)v) + offset, d); \
//	return SPQ<r.Dim()[0], r.Dim()[1], r.Dim()[2], r.Dim()[3], r.Dim()[4], offset>(v * factor) \
//	} \
//	static constexpr auto operator ""_##symbol_no_quotes(unsigned long long v) \
//{ \
//	signed char d[(int)PhysicalQuantity::QuantityType::ENUM_MAX]  \
//{Ma, Di, Ti, Te, Cu}; \
//	auto r = return PhysicalQuantity(((PhysicalQuantity::num)factor * (PhysicalQuantity::num)v) + offset, d); \
//	return SPQ<r.dim()[0], r.dim()[1], r.dim()[2], r.dim()[3], r.dim()[4], offset>(v * factor) \ \
//	}
///*#pragma message("Compiling literal symbol " #symbol_no_quotes) */ \

//----------------------------------------------------------------------------
#define CxLiteralWithPrefixes(symbol_no_quotes, Ma, Di, Ti, Te, Cu, factor, iofs) \
	CxLiteral( symbol_no_quotes,  Ma, Di, Ti, Te, Cu, factor, iofs) \
	CxLiteral( c##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-2  , iofs) \
	CxLiteral( k##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e3   , iofs) \
	CxLiteral( m##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-3  , iofs) \
	CxLiteral( M##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e6   , iofs) \
	CxLiteral( u##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-6  , iofs) \
	CxLiteral( G##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e9   , iofs) \
	CxLiteral( n##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-9  , iofs) \
	CxLiteral( T##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e12  , iofs) \
	CxLiteral( p##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-12 , iofs) \
	CxLiteral( P##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e15  , iofs) \
	CxLiteral(da##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 10.0  , iofs) \
	CxLiteral( f##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-15 , iofs) \
	CxLiteral( d##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 0.1   , iofs) \
	CxLiteral( h##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 100.0 , iofs) \
	CxLiteral( E##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e18  , iofs) \
	CxLiteral( a##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-18 , iofs) \
	CxLiteral( z##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-21 , iofs) \
	CxLiteral( Z##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e21  , iofs) \
	CxLiteral( y##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-24 , iofs) \
	CxLiteral( Y##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e24  , iofs) \

#define CxLiteralWithPrefixesNoBase(symbol_no_quotes, Ma, Di, Ti, Te, Cu, factor, iofs) \
	CxLiteral( c##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-2  , iofs) \
	CxLiteral( k##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e3   , iofs) \
	CxLiteral( m##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-3  , iofs) \
	CxLiteral( M##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e6   , iofs) \
	CxLiteral( u##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-6  , iofs) \
	CxLiteral( G##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e9   , iofs) \
	CxLiteral( n##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-9  , iofs) \
	CxLiteral( T##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e12  , iofs) \
	CxLiteral( p##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-12 , iofs) \
	CxLiteral( P##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e15  , iofs) \
	CxLiteral(da##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 10.0  , iofs) \
	CxLiteral( f##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-15 , iofs) \
	CxLiteral( d##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 0.1   , iofs) \
	CxLiteral( h##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 100.0 , iofs) \
	CxLiteral( E##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e18  , iofs) \
	CxLiteral( a##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-18 , iofs) \
	CxLiteral( z##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-21 , iofs) \
	CxLiteral( Z##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e21  , iofs) \
	CxLiteral( y##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-24 , iofs) \
	CxLiteral( Y##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e24  , iofs) \
//----------------------------------------------------------------------------

#else // defined(YES_CONSTEXPR) && !defined(NO_INLINE)
#define CxLiteral(DISABLED, YES_CONSTEXPR, NOT, DEFINED, OR, NO_INLINE, ppOptions_h)
#endif //#if defined(YES_CONSTEXPR)
#endif // !NO_LITERALS
// End literals
//==================================================================================

#ifndef PQ_GENCODE
#include <PhysicalQuantity/literals.h>
#endif


// Useful for testing if there are any unclosed #if blocks
//#pragma message("Hello from the end of PhysicalQuantity.h")
