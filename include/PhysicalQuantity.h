#pragma once

#include <PhysicalQuantity/ppOptions.h>


#if !defined(NO_INLINE) && !defined(INLINE_KEYWORD)
#define INLINE_KEYWORD __inline
#endif //#if !defined(NO_INLINE) && !defined(INLINE_KEYWORD)

//==================================================================================
// Config checking
#ifdef NO_NEW
#define PQ_HAS_NO_NEW 0x01
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
#ifdef NO_PRINTF
#define PQ_HAS_NO_PRINTF 0x40
#else
#define PQ_HAS_NO_PRINTF 0
#endif
#define PQ_HEADER_OPTIONS (PQ_HAS_NO_NEW | PQ_HAS_NO_STD_STRING | PQ_HAS_NO_LITERALS | PQ_HAS_NO_INLINE | PQ_HAS_NO_HASHING | PQ_HAS_NO_THROW | PQ_HAS_NO_PRINTF)
// End config checking
//==================================================================================

// csubstr.h is included in the PhysicalQuantity project's include/ directory.
// This path should be included in project settings or as an -I in makefile
#include <csubstr.h>

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

class PhysicalQuantity
{
public:
	
	// Base numeric type
	typedef double num;
	//typedef float num;

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
	struct cstrHasherTiny
	{
	public:
		size_t operator()(const CSubString& s) const;
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
		E_HEADER_CONFIG
	};
	static void (*errorHandler)(void* userContext, ErrorCode e);
	static void* errorUserContext;
#ifndef NO_INLINE
	INLINE_KEYWORD static void SetErrorHandler(void (*errorHandler_arg)(void* context, ErrorCode e)) { errorHandler = errorHandler_arg; }
#else
	static void SetErrorHandler(void (*errorHandler_arg)(void* context, ErrorCode e)) { errorHandler = errorHandler_arg; }
#endif
#else
	class UnitMismatchException : public std::exception
	{
	public:
		UnitMismatchException();
		UnitMismatchException(const char* message);
	};
	class InvalidExpressionException : public std::exception
	{
	public:
		InvalidExpressionException();
		InvalidExpressionException(const char* message);
	};
	class HeaderConfigException : public std::exception
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
	class PreferredUnitsBase
	{
		friend class PhysicalQuantity;
	public:
		struct UnitPref
		{
			int iUnit;
			int iPrefix;
		};
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
	};

#ifndef NO_NEW
	class PreferredUnits_dynamic : public PreferredUnitsBase
	{
	public:
		PreferredUnits_dynamic(const CSubString& unitList_SpaceDelimited);
		~PreferredUnits_dynamic();
	};
#else
	class PreferredUnits_static : public PreferredUnitsBase
	{
	public:
		PreferredUnits_static(const CSubString& unitList_SpaceDelimited, void* storage, size_t storageSizeBytes);
	};
#endif
#ifdef NO_NEW
	typedef PreferredUnits_static PreferredUnits;
#else
	typedef PreferredUnits_dynamic PreferredUnits;
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
	PhysicalQuantity& operator=(num value);
	PhysicalQuantity& operator=(const PhysicalQuantity& cp);
	PhysicalQuantity(const char* str);
	~PhysicalQuantity() = default;

	void parse(const CSubString& text);
	num convert(const CSubString& units) const;
	size_t sprint(char* buf, int size, const PreferredUnitsBase& pu) const;
	size_t sprint(char* buf, int size) const;
#ifndef NO_STD_STRING
	std::string toString() const;
	std::string toString(const PreferredUnitsBase&) const;
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


	static bool findUnit(CSubString name, int& outUnitIndex, int& outPrefixIndex);
	static bool feq(num a, num b, num toleranceFactor);

	static const num Pi;
	static const int compiledHeaderOptions;
	static const UnitDefinition KnownUnits[];
	static const int KnownUnitsLength;
	static const Prefix KnownPrefixes[];
	static const int KnownPrefixesLength;
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

public:
#ifdef NO_INLINE
	static void parseUnits(const char* unitStr, signed char(&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut);
	num convert(const char* units) const;
	static bool findUnit(const char* pcharName, int& outUnitIndex, int& outPrefixIndex);
	bool sameUnitAs(const PhysicalQuantity& rhs) const;
	bool unitsMatch(const PhysicalQuantity& rhs) const;
	void parse(const char* text);

#else
	INLINE_KEYWORD static void parseUnits(const char* unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut) { return parseUnits(CSubString(unitStr), unitsOut, factorOut, offsetOut); }
	INLINE_KEYWORD num convert(const char* units) const { return convert(csubstr(units)); }
	INLINE_KEYWORD static bool findUnit(const char* pcharName, int& outUnitIndex, int& outPrefixIndex) { return findUnit(CSubString(pcharName), outUnitIndex, outPrefixIndex); }
	INLINE_KEYWORD bool sameUnitAs(const PhysicalQuantity& rhs) const { return isLikeQuantity(rhs); }
	INLINE_KEYWORD bool unitsMatch(const PhysicalQuantity& rhs) const { return isLikeQuantity(rhs); }
	INLINE_KEYWORD void parse(const char* text) { parse(csubstr(text)); }

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

DeclareLiteral(m)
DeclareLiteral(g)
DeclareLiteral(s)
DeclareLiteral(Hz)
DeclareLiteral(N)
DeclareLiteral(J)
DeclareLiteral(K)
DeclareLiteral(C)
DeclareLiteral(ang)

#endif // !NO_LITERALS
// End literals
//==================================================================================
