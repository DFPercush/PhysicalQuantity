#pragma once

#include <PhysicalQuantity/ppOptions.h>


#if !defined(NO_INLINE) && !defined(INLINE_KEYWORD)
#define INLINE_KEYWORD __inline
#endif //#if !defined(NO_INLINE) && !defined(INLINE_KEYWORD)

//==================================================================================
// Config checking
// TODO: NO_TYPEDEFS
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
#ifdef CPP11
#define PQ_HAS_CPP11 0x80
#else
#define PQ_HAS_CPP11 0
#endif
// Typedefs wouldn't affect anything at the linker/binary stage, would they?
//#ifdef NO_TYPEDEFS
//#define PQ_HAS_NO_TYPEDEFS 0x100
//#else
#define PQ_HAS_NO_TYPEDEFS 0
//#endif

#define PQ_HEADER_OPTIONS (PQ_HAS_NO_NEW | PQ_HAS_NO_STD_STRING | PQ_HAS_NO_LITERALS | PQ_HAS_NO_INLINE | PQ_HAS_NO_HASHING | PQ_HAS_NO_THROW | PQ_HAS_CPP11 | PQ_HAS_NO_TYPEDEFS)

// End config checking
//==================================================================================


// Should be getting this from ppOptions.h, but it really, *really* needs to be defined for sure
#ifndef MAX_NUM_TEXT_LENGTH
#define MAX_NUM_TEXT_LENGTH 26
#endif


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

// end standard library dependencies
//========================================


class PhysicalQuantity
{
public:
	
	//======================
	// Base numeric type
	typedef double num;
	//typedef float num;
	//======================


	//==================================================================================
	// CSubString
	class CSubString
	{
	private:
		const char* str;
		int start;
		int end; // constructor takes a length to emulate standard behavior, but internally end is easier
	public:
		CSubString();
		CSubString(const char* str_arg, int start_arg = 0, int len_arg = -1);
		CSubString(const CSubString& from, int start_arg = 0, int len_arg = -1);
		CSubString& operator=(const CSubString& cp);
		bool operator== (const char* cmp) const;
		bool operator==(const CSubString& cmp) const;
		bool begins(const CSubString& test) const;
		bool ends(const CSubString& test) const;
		int at(const char* test, int start = 0) const; // where this needle appears in a haystack argument, or -1
		bool copy(char* buf, int size) const;
		int find_first_of(const CSubString& find, int startOfs = 0) const;
		int find_first_not_of(const CSubString& find, int startOfs = 0) const;
		int find_first_of(char c, int startOfs = 0) const;
		int find_first_not_of(char c, int startOfs = 0) const;
		int find_last_of(const CSubString& find, int startOfs = -1) const;
		int find_last_not_of(const CSubString& find, int startOfs = -1) const;
		// TODO: const char* inline
		int find_last_of(char c, int startOfs = -1) const;
		int find_last_not_of(char c, int startOfs = -1) const;
		int atoi();
		double atof();
		CSubString substr(int startArg = 0, int lenArg = -1) const;
		bool isnumber() const;
		bool isint() const;
		CSubString trim() const;

#ifdef NO_INLINE
		char operator[](int index) const;
		bool begins(const char* test) const;
		bool ends(const char* test) const;
		int size() const;
		int length() const;
		int find_first_of(const char* find, int startOfs = 0) const;
		int find_first_not_of(const char* find, int startOfs = 0) const;
#else
		INLINE_KEYWORD char operator[](int index) const { if (index < 0 || start + index >= end) { return 0; } return str[start + index];}
		INLINE_KEYWORD bool begins(const char* test) const { return begins(CSubString(test)); }
		INLINE_KEYWORD bool ends(const char* test) const { return ends(CSubString(test)); }
		INLINE_KEYWORD int size() const { return end - start; }
		INLINE_KEYWORD int length() const { return (end - start) >= 0 ? (end-start) : 0 ; }
		INLINE_KEYWORD int find_first_of(const char* find, int startOfs = 0) const { return find_first_of(CSubString(find), startOfs); }
		INLINE_KEYWORD int find_first_not_of(const char* find, int startOfs = 0) const { return find_first_not_of(CSubString(find), startOfs); }
#endif
	};
	// End CSubString
	//==================================================================================



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

	struct UnitDefinition
	{
		const char* symbol;
		const char* longName;
		const char* plural; // TODO: implement. Maybe #define away? Or the whole thing.
		num factor;
		const signed char dim[(int)QuantityType::ENUM_MAX];
		unsigned short flags = 0;
	};
#define NOPREFIX  0x01
#define CANPREFIX 0
#define NOLITERAL 0x02
#define MAKELITERAL 0


	struct Prefix
	{
		const char* symbol;
		const char* longName;
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
#else //PQ_GENCODE
	// Actual values are written by gencode to hashParams.acpp
	static const int hashTableSize_UnitSymbols;
	static const int hashTableSize_UnitLongNames;
	static const int hashTableSize_PrefixSymbols;
	static const int hashTableSeed_UnitSymbols;
	static const int hashTableSeed_UnitLongNames;
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
	class UnitListBase
	{
		friend class PhysicalQuantity;
	public:
		struct UnitPref
		{
			unitIndex_t iUnit;
			prefixIndex_t iPrefix;
			unitPower_t power; // TODO
		};
		virtual ~UnitListBase();
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
		virtual ~UnitList_dynamic();
	};
#endif //#ifndef NO_NEW
	class UnitList_static : public UnitListBase
	{
	public:
		UnitList_static(const CSubString& unitList_SpaceDelimited, void* storage, size_t storageSizeBytes);
		UnitList_static(const CSubString& unitList_SpaceDelimited);
	};
#ifdef NO_NEW
	typedef UnitList_static UnitList;
#else
	typedef UnitList_dynamic UnitList;
#endif
	// End formatting stuff
	//==================================================================================


	//==================================================================================
	// Main class members
	PhysicalQuantity();
	PhysicalQuantity(const PhysicalQuantity& copy);
	PhysicalQuantity(PhysicalQuantity&& move) noexcept;
	PhysicalQuantity(num value, const char* unit);
	PhysicalQuantity(num value);
	PhysicalQuantity(CSubString str);
#if defined(CPP11)
	constexpr PhysicalQuantity(num value_p, signed char dim_p[(int)QuantityType::ENUM_MAX])
		: value(value_p), dim {dim_p[0], dim_p[1], dim_p[2], dim_p[3], dim_p[4]} {}
#else
	PhysicalQuantity(num value_p, signed char dim_p[(int)QuantityType::ENUM_MAX]);
#endif
	PhysicalQuantity& operator=(num value);
	PhysicalQuantity& operator=(const PhysicalQuantity& cp);
	~PhysicalQuantity() = default;

	// TODO: NO_TEXT
	void parse(const CSubString& text);
	num convert(const CSubString& units) const;
	size_t sprint(char* buf, size_t size, const UnitListBase& pu, bool useSlash = true) const;
	size_t sprint(char* buf, size_t size, const CSubString& sspu, bool useSlash = true) const; // TODO: inline
	size_t sprint(char* buf, size_t size, const CSubString& sspu, void* puBuf, size_t puBufSize, bool useSlash = true) const;

#ifdef NO_INLINE
	static PhysicalQuantity get1kg();
	static PhysicalQuantity get1m();
	static PhysicalQuantity get1s();
	static PhysicalQuantity get1K();
	static PhysicalQuantity get1A();
	bool isScalar();
#else // NO_INLINE
	static PhysicalQuantity get1kg() { char d[5]={1,0,0,0,0}; return PhysicalQuantity(1.0, d); }
	static PhysicalQuantity get1m() { char d[5]={0,1,0,0,0}; return PhysicalQuantity(1.0, d); }
	static PhysicalQuantity get1s() { char d[5]={0,0,1,0,0}; return PhysicalQuantity(1.0, d); }
	static PhysicalQuantity get1K() { char d[5]={0,0,0,1,0}; return PhysicalQuantity(1.0, d); }
	static PhysicalQuantity get1A() { char d[5]={0,0,0,0,1}; return PhysicalQuantity(1.0, d); }
	bool isScalar() { return (dim[0] == 0 && dim[1] == 0 && dim[2] == 0 && dim[3] == 0 && dim[5] == 0); }
#endif // #else of #ifdef NO_INLINE


#ifndef NO_STD_STRING
	std::string toString() const;
	std::string toString(const UnitListBase&) const;
#endif

#if defined(CPP11)
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
		return PhysicalQuantity(value * rhs.value, d);
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
		return PhysicalQuantity(value / rhs.value, d);
	}
#else
//#endif //#if defined(CPP11)
	PhysicalQuantity operator* (const PhysicalQuantity& rhs) const;
	PhysicalQuantity operator/ (const PhysicalQuantity& rhs) const;
#endif //#if defined(CPP11)

	PhysicalQuantity operator+ (const PhysicalQuantity& rhs) const;
	PhysicalQuantity operator- (const PhysicalQuantity& rhs) const;
	PhysicalQuantity pow(int exp);

#if defined(CPP11)
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
#else //#if defined(CPP11)
	PhysicalQuantity operator* (num rhs) const;
	PhysicalQuantity operator/ (num rhs) const;
#endif //#if defined(CPP11)
	PhysicalQuantity operator+ (num rhs) const;
	PhysicalQuantity operator- (num rhs) const;

	bool operator==(const PhysicalQuantity& rhs) const;
	bool operator>=(const PhysicalQuantity& rhs) const;
	bool operator<=(const PhysicalQuantity& rhs) const;
	bool operator!=(const PhysicalQuantity& rhs) const;

	bool like(const PhysicalQuantity& rhs) const; // Same kind of quantity.

#ifndef NO_NETWORK
	bool writeNetworkBinary(void* buf, size_t size);
	bool readNetworkBinary(void* buf);
#endif

	static bool findUnit(CSubString name, unitIndex_t& outUnitIndex, prefixIndex_t& outPrefixIndex);
	static bool feq(num a, num b, num toleranceFactor);
	static PhysicalQuantity eval(CSubString str);

	static const int compiledHeaderOptions;
	static const UnitDefinition KnownUnits[];
	static const int KnownUnitsLength;
	static const num KnownUnitOffsets[];
	static const int KnownUnitOffsetsLength;

	static const Prefix KnownPrefixes[];
	static const int KnownPrefixesLength;
	static const prefixIndex_t dekaIndex;
	static num equalityToleranceFactor;

private:
	num value;
	signed char dim[(int)QuantityType::ENUM_MAX];

	void init();
	static void parseUnits(const CSubString& unitStr, signed char (&dimOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut); // throws if unknown/invalid unit
	static void mulUnit(signed char (&dimOut)[(int)QuantityType::ENUM_MAX], const UnitDefinition& unit, signed int power, bool invert = false); // deals only with quantity dimension, conversion factors are handled elsewhere
	unsigned int magdim() const; // Magnitude of dimension = sum(abs(dim[x]))
	int magdimReduce(const UnitDefinition& unit) const;  // Divide by what power of (unit) to minimize magdim? Used in text output logic.

	// in conjunction with sprint()
	void sprintHalf(PhysicalQuantity& r, const UnitListBase & pu, bool& hasDenom, bool inDenomNow, int &md, int origmd, bool useSlash, size_t &outofs, size_t size, char * buf) const;
	//void sprintHalf(const UnitListBase & pu, int &md, PhysicalQuantity &r, int origmd, bool useSlash, int &outofs, int size, char * buf) const;
	void sprintHalfTryUnit(int iTestUnit, PhysicalQuantity & r, int origmd, bool & hasDenom, bool useSlash, bool inDenomNow, int plen, size_t & outofs, size_t size, char * buf, prefixIndex_t ipre, int & md, bool preferred) const;
	void WriteOutputUnit(int plen, int ulen, int reduceExp, size_t &outofs, size_t size, char * buf, prefixIndex_t ipre, const PhysicalQuantity::UnitDefinition & testunit) const;

public:
#ifdef NO_INLINE
	PhysicalQuantity(const char* str);
	static void parseUnits(const char* unitStr, signed char(&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut);
	num convert(const char* units) const;
	static bool findUnit(const char* pcharName, unitIndex_t& outUnitIndex, prefixIndex_t& outPrefixIndex);
	//bool sameUnitAs(const PhysicalQuantity& rhs) const;
	//bool unitsMatch(const PhysicalQuantity& rhs) const;
	void parse(const char* text);
	//size_t sprint(char* buf, size_t size, const char* pu, bool useSlash = true) const;
	size_t sprint(char* buf, size_t size, bool useSlash = true) const;

#else
	INLINE_KEYWORD 	PhysicalQuantity(const char* str) { PhysicalQuantity(CSubString(str)); }
	INLINE_KEYWORD static void parseUnits(const char* unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut) { return parseUnits(CSubString(unitStr), unitsOut, factorOut, offsetOut); }
	INLINE_KEYWORD num convert(const char* units) const { return convert(CSubString(units)); }
	INLINE_KEYWORD static bool findUnit(const char* pcharName, unitIndex_t& outUnitIndex, prefixIndex_t& outPrefixIndex) { return findUnit(CSubString(pcharName), outUnitIndex, outPrefixIndex); }
	//INLINE_KEYWORD bool sameUnitAs(const PhysicalQuantity& rhs) const { return isLikeQuantity(rhs); }
	//INLINE_KEYWORD bool unitsMatch(const PhysicalQuantity& rhs) const { return isLikeQuantity(rhs); }
	INLINE_KEYWORD void parse(const char* text) { parse(CSubString(text)); }
	//INLINE_KEYWORD size_t sprint(char* buf, int size, const char* pu, bool useSlash = true) const { return sprint(buf, size, UnitList(pu), useSlash); }
	INLINE_KEYWORD size_t sprint(char* buf, size_t size, bool useSlash = true) const { return sprint(buf, size, UnitList(""), useSlash); }

#endif
	// End main class members
	//==================================================================================
};

#define PQHeaderOptionsMatch (PQ_HEADER_OPTIONS == PhysicalQuantity::compiledHeaderOptions)


//==================================================================================
// Literals
#ifndef NO_LITERALS

// Use these in a header.
#define DeclareLiteral(symbol_no_quotes) PhysicalQuantity operator ""_##symbol_no_quotes(long double); PhysicalQuantity operator ""_##symbol_no_quotes(unsigned long long);
#define DeclareLiteralWithPrefixes(symbol_no_quotes) \
	DeclareLiteral(  symbol_no_quotes) \
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


#if defined(CPP11)
// Implement the fundamental SI units of each quantity as compile time constants

// Use these in a header
#define CxLiteral(symbol_no_quotes, Ma, Di, Ti, Te, Cu, factor, offset) \
	constexpr PhysicalQuantity operator ""_##symbol_no_quotes(long double v) \
	{ \
		signed char d[(int)PhysicalQuantity::QuantityType::ENUM_MAX]  \
		{Ma, Di, Ti, Te, Cu}; \
		return PhysicalQuantity(((PhysicalQuantity::num)factor * (PhysicalQuantity::num)v) + offset, d); \
	} \
	constexpr PhysicalQuantity operator ""_##symbol_no_quotes(unsigned long long v) \
	{ \
		signed char d[(int)PhysicalQuantity::QuantityType::ENUM_MAX]  \
		{Ma, Di, Ti, Te, Cu}; \
		return PhysicalQuantity(((PhysicalQuantity::num)factor * (PhysicalQuantity::num)v) + offset, d); \
	}

//----------------------------------------------------------------------------
#define BLANK(x)
#define CxLiteralWithPrefixes(symbol_no_quotes, Ma, Di, Ti, Te, Cu, factor, offset) \
	CxLiteral( symbol_no_quotes,  Ma, Di, Ti, Te, Cu, factor, offset) \
	CxLiteral( c##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-2  , offset) \
	CxLiteral( k##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e3   , offset) \
	CxLiteral( m##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-3  , offset) \
	CxLiteral( M##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e6   , offset) \
	CxLiteral( u##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-6  , offset) \
	CxLiteral( G##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e9   , offset) \
	CxLiteral( n##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-9  , offset) \
	CxLiteral( T##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e12  , offset) \
	CxLiteral( p##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-12 , offset) \
	CxLiteral( P##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e15  , offset) \
	CxLiteral(da##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 10.0  , offset) \
	CxLiteral( f##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-15 , offset) \
	CxLiteral( d##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 0.1   , offset) \
	CxLiteral( h##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 100.0 , offset) \
	CxLiteral( E##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e18  , offset) \
	CxLiteral( a##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-18 , offset) \
	CxLiteral( z##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-21 , offset) \
	CxLiteral( Z##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e21  , offset) \
	CxLiteral( y##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e-24 , offset) \
	CxLiteral( Y##symbol_no_quotes, Ma, Di, Ti, Te, Cu,  factor * 1e24  , offset) \
//----------------------------------------------------------------------------

#else // defined(CPP11) && !defined(NO_INLINE)
#define CxLiteral(DISABLED, CPP11, NOT, DEFINED, OR, NO_INLINE, ppOptions_h)
#endif //#if defined(CPP11)
#endif // !NO_LITERALS
// End literals
//==================================================================================

#ifndef PQ_GENCODE
#include <PhysicalQuantity/literals.ah>
#endif

#ifndef NO_TYPEDEFS
typedef PhysicalQuantity PQ;
typedef PhysicalQuantity::CSubString csubstr;
#endif

// Useful for testing if there are any unclosed #if blocks
//#pragma message("Hello from the end of PhysicalQuantity.h")
