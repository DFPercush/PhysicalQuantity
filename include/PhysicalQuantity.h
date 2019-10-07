#pragma once

#include <PhysicalQuantity/ppOptions.h>


#if !defined(NO_INLINE) && !defined(INLINE_KEYWORD)
#define INLINE_KEYWORD __inline
#endif //#if !defined(NO_INLINE) && !defined(INLINE_KEYWORD)

//==================================================================================
// Config checking
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
#define PQ_HEADER_OPTIONS (PQ_HAS_NO_NEW | PQ_HAS_NO_STD_STRING | PQ_HAS_NO_LITERALS | PQ_HAS_NO_INLINE | PQ_HAS_NO_HASHING | PQ_HAS_NO_THROW)
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
		CHARGE,
		ENUM_MAX
	};

	struct UnitDefinition
	{
		const char* symbol;
		const char* longName;
		const signed char dim[(int)QuantityType::ENUM_MAX];
		num offset; // temperature is a special snowflake
		num factor;
	};

	struct Prefix
	{
		const char* symbol;
		const char* longName;
		num factor;
	};
	// End unit and dimension stuff
	//==================================================================================


	//==================================================================================
	// Hashing function 
	//    originally to be used as a template parameter to unordered_map,
	//    but that wastes a lot of RAM
#ifndef NO_HASHING
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
	class UnitListBase
	{
		friend class PhysicalQuantity;
	public:
		struct UnitPref
		{
			unsigned short iUnit;
			signed char iPrefix;
			signed char power; // TODO
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
	typedef UnitList_static PreferredUnits;
#else
	typedef UnitList_dynamic PreferredUnits;
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
	PhysicalQuantity& operator=(num value);
	PhysicalQuantity& operator=(const PhysicalQuantity& cp);
	~PhysicalQuantity() = default;

	void parse(const CSubString& text);
	num convert(const CSubString& units) const;
	size_t sprint(char* buf, size_t size, const UnitListBase& pu, bool useSlash = true) const;
	size_t sprint(char* buf, size_t size, const CSubString& sspu, bool useSlash = true) const; // TODO: inline
	size_t sprint(char* buf, size_t size, const CSubString& sspu, void* puBuf, size_t puBufSize, bool useSlash = true) const;

#ifndef NO_STD_STRING
	std::string toString() const;
	std::string toString(const UnitListBase&) const;
#endif

	PhysicalQuantity operator* (const PhysicalQuantity& rhs) const;
	PhysicalQuantity operator/ (const PhysicalQuantity& rhs) const;
	PhysicalQuantity operator+ (const PhysicalQuantity& rhs) const;
	PhysicalQuantity operator- (const PhysicalQuantity& rhs) const;

	PhysicalQuantity operator* (num rhs) const;
	PhysicalQuantity operator/ (num rhs) const;
	PhysicalQuantity operator+ (num rhs) const;
	PhysicalQuantity operator- (num rhs) const;

	bool operator==(const PhysicalQuantity& rhs) const;
	// TODO: boolean comparison operators
	
	bool isLikeQuantity(const PhysicalQuantity& rhs) const;

#ifndef NO_NETWORK
	bool writeNetworkBinary(void* buf, size_t size);
	bool readNetworkBinary(void* buf);
#endif

	static bool findUnit(CSubString name, int& outUnitIndex, int& outPrefixIndex);
	static bool feq(num a, num b, num toleranceFactor);
	static PhysicalQuantity eval(CSubString str);

	static const num Pi;
	static const int compiledHeaderOptions;
	static const UnitDefinition KnownUnits[];
	static const int KnownUnitsLength;
	static const Prefix KnownPrefixes[];
	static const int KnownPrefixesLength;
	static const int dekaIndex;
#ifndef NO_HASHING
	// Hash table sizes are defined in hash.cpp
	static const int hashTableSize_UnitSymbols;
	static const int hashTableSize_UnitLongNames;
	static const int hashTableSize_PrefixSymbols;
	static const int hashTableSize_PrefixLongNames;
#endif //#ifndef NO_HASHING
	static num equalityToleranceFactor;

private:
	num value;
	signed char dim[(int)QuantityType::ENUM_MAX];

	void init();
	static void parseUnits(const CSubString& unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut); // throws if unknown/invalid unit
	static void mulUnit(signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], const UnitDefinition& unit, signed int power, bool invert = false); // deals only with quantity dimension, conversion factors are handled elsewhere
	unsigned int magdim() const; // Magnitude of dimension = sum(abs(dim[x]))
	int magdimReduce(const UnitDefinition& unit) const;  // Divide by what power of (unit) to minimize magdim? Used in text output logic.

	// in conjunction with sprint()
	void sprintHalf(PhysicalQuantity& r, const UnitListBase & pu, bool& hasDenom, bool inDenomNow, int &md, int origmd, bool useSlash, size_t &outofs, size_t size, char * buf) const;
	//void sprintHalf(const UnitListBase & pu, int &md, PhysicalQuantity &r, int origmd, bool useSlash, int &outofs, int size, char * buf) const;
	void sprintHalfTryUnit(const PhysicalQuantity::UnitDefinition & testunit, PhysicalQuantity & r, int origmd, bool & hasDenom, bool useSlash, bool inDenomNow, int plen, size_t & outofs, size_t size, char * buf, int ipre, int & md, bool preferred) const;
	void WriteOutputUnit(int plen, int ulen, int reduceExp, size_t &outofs, size_t size, char * buf, int ipre, const PhysicalQuantity::UnitDefinition & testunit) const;

public:
#ifdef NO_INLINE
	PhysicalQuantity(const char* str);
	static void parseUnits(const char* unitStr, signed char(&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut);
	num convert(const char* units) const;
	static bool findUnit(const char* pcharName, int& outUnitIndex, int& outPrefixIndex);
	bool sameUnitAs(const PhysicalQuantity& rhs) const;
	bool unitsMatch(const PhysicalQuantity& rhs) const;
	void parse(const char* text);
	//size_t sprint(char* buf, size_t size, const char* pu, bool useSlash = true) const;
	size_t sprint(char* buf, size_t size, bool useSlash = true) const;

#else
	INLINE_KEYWORD 	PhysicalQuantity(const char* str) { PhysicalQuantity(CSubString(str)); }
	INLINE_KEYWORD static void parseUnits(const char* unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut) { return parseUnits(CSubString(unitStr), unitsOut, factorOut, offsetOut); }
	INLINE_KEYWORD num convert(const char* units) const { return convert(CSubString(units)); }
	INLINE_KEYWORD static bool findUnit(const char* pcharName, int& outUnitIndex, int& outPrefixIndex) { return findUnit(CSubString(pcharName), outUnitIndex, outPrefixIndex); }
	INLINE_KEYWORD bool sameUnitAs(const PhysicalQuantity& rhs) const { return isLikeQuantity(rhs); }
	INLINE_KEYWORD bool unitsMatch(const PhysicalQuantity& rhs) const { return isLikeQuantity(rhs); }
	INLINE_KEYWORD void parse(const char* text) { parse(CSubString(text)); }
	//INLINE_KEYWORD size_t sprint(char* buf, int size, const char* pu, bool useSlash = true) const { return sprint(buf, size, PreferredUnits(pu), useSlash); }
	INLINE_KEYWORD size_t sprint(char* buf, size_t size, bool useSlash = true) const { return sprint(buf, size, PreferredUnits(""), useSlash); }

#endif
	// End main class members
	//==================================================================================
};

#define PQHeaderOptionsMatch (PQ_HEADER_OPTIONS == PhysicalQuantity::compiledHeaderOptions)


//==================================================================================
// Literals
#ifndef NO_LITERALS
// Declare literals
// TODO: Also declare with all prefixes?
#define DeclareLiteral(symbol) PhysicalQuantity operator ""_##symbol(long double); PhysicalQuantity operator ""_##symbol(unsigned long long);

DeclareLiteral(rad)
DeclareLiteral(deg)
DeclareLiteral(m)
DeclareLiteral(g)
DeclareLiteral(s)
DeclareLiteral(Hz)
DeclareLiteral(N)
DeclareLiteral(J)
DeclareLiteral(K)
DeclareLiteral(C)
DeclareLiteral(ang)
DeclareLiteral(lb)

#endif // !NO_LITERALS
// End literals
//==================================================================================
