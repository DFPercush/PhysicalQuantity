#pragma once

// Preprocessor options:
//#define NO_INLINE
//#define NO_HASHING
//#define NO_THROW
//#define NO_PRINTF
//#define NO_NEW
//#define NO_LITERALS

#ifndef NO_THROW
#include <exception>
#include <stdexcept>
#endif
#ifndef NO_NEW
#include <vector>
//#include <unordered_map>
#endif

// TODO: not depend on std::strings
//#include <string>

//class CSubString;
class CSubString
{
private:
	const char* str;
	int start;
	int end; // length in constructor, internally end is easier
public:
	//typedef double num; //Duplicate typedef, meh

	CSubString();
	CSubString(const char* str_arg, int start_arg = 0, int len_arg = -1);
	//CSubString(const CSubString& cp);
	CSubString(const CSubString& from, int start_arg = 0, int len_arg = -1);
	__inline char operator[](int index) const { if (start + index >= end) { return 0; } return str[start + index];}
	CSubString& operator=(const CSubString& cp);
	bool operator== (const char* cmp) const;
	bool operator==(const CSubString& cmp) const;
	bool begins(const CSubString& test) const;
	bool ends(const CSubString& test) const;
	__inline bool begins(const char* test) const { return begins(CSubString(test)); }
	__inline bool ends(const char* test) const { return ends(CSubString(test)); }
	int at(const char* test, int start = 0) const; // where this needle appears in a haystack argument, or -1
	__inline int size() const { return end - start; }
	__inline int length() const { return end - start; }
	bool copy(char* buf, int size) const;
	int find_first_of(const CSubString& find, int startOfs = 0) const;
	int find_first_not_of(const CSubString& find, int startOfs = 0) const;
	__inline int find_first_of(const char* find, int startOfs = 0) const { return find_first_of(CSubString(find), startOfs); }
	__inline int find_first_not_of(const char* find, int startOfs = 0) const { return find_first_not_of(CSubString(find), startOfs); }
	int find_first_of(char c) const;
	int find_first_not_of(char c) const;
	double atof(); // Not thread safe. Modifies original string temporarily.
};
typedef CSubString csubstr;

class PhysicalQuantity
{
public:
	
	// Base numeric type
	typedef double num;

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


#ifdef NO_THROW
	enum ErrorCode
	{
		E_SUCCESS = 0,
		E_UNIT_MISMATCH,
		E_INVALID_EXPRESSION,
		E_MEMORY
	};
	static void (*errorHandler)(void* userContext, ErrorCode e);
	static void* errorUserContext;
	__inline static void SetErrorHandler(void (*errorHandler_arg)(void* context, ErrorCode e)) { errorHandler = errorHandler_arg; }
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
#endif  // !NO_THROW

	class PreferredUnitsBase
	{
		friend class PhysicalQuantity;
	protected:
		int* unitIndeces;
		int count_;
		__inline int count() { return count_; }
		const UnitDefinition& operator[] (int i);
		void build(const CSubString& unitList, int* buffer, int bufferLen, bool dynamic);
	};

#ifndef NO_NEW
	class PreferredUnits_dynamic : public PreferredUnitsBase
	{
	public:
		PreferredUnits_dynamic(const CSubString& unitList_SpaceDelimited);
		~PreferredUnits_dynamic();
	};
#endif
	class PreferredUnits_static : public PreferredUnitsBase
	{
	public:
		PreferredUnits_static(const CSubString& unitList, int* storage, size_t storageSizeBytes);
	};
#ifdef NO_NEW
	typedef PreferredUnits_static PreferredUnits;
#else
	typedef PreferredUnits_dynamic PreferredUnits;
#endif
	
	static const num Pi;

	PhysicalQuantity();
	PhysicalQuantity(const PhysicalQuantity& copy);
	PhysicalQuantity(PhysicalQuantity&& move) noexcept;
	PhysicalQuantity(num value, const char* unit);
	PhysicalQuantity(const char* str);
	//PhysicalQuantity(num value, const std::vector<UnitQty>& units);
	//PhysicalQuantity(num value, std::vector<UnitQty>&& units);
	~PhysicalQuantity() = default;

	// To read the value, specify the unit, must match
	//num convert(const std::vector<UnitQty> unit);
	num convert(const std::string& units);

	//void parse(CSubString)
	void parse(const char* text);
	//void parse(std::string text);
	//__inline void parse(std::string text) { parse(text.c_str()); }

#ifndef NO_NEW
	std::string toString();
	std::string toString(const PreferredUnitsBase&);
#endif

	PhysicalQuantity operator* (const PhysicalQuantity& rhs);
	PhysicalQuantity operator/ (const PhysicalQuantity& rhs);
	PhysicalQuantity operator+ (const PhysicalQuantity& rhs);
	PhysicalQuantity operator- (const PhysicalQuantity& rhs);

	PhysicalQuantity operator* (num rhs);
	PhysicalQuantity operator/ (num rhs);
	PhysicalQuantity operator+ (num rhs);
	PhysicalQuantity operator- (num rhs);

	bool operator==(const PhysicalQuantity& rhs);
	// TODO: boolean comparison operators
	
	bool isLikeQuantity(const PhysicalQuantity& rhs);
	__inline bool sameUnitAs(const PhysicalQuantity& rhs) { return isLikeQuantity(rhs); }
	__inline bool unitsMatch(const PhysicalQuantity& rhs) { return isLikeQuantity(rhs); }

	class cstrLess
	{
	public:
		bool operator() (const char* a, const char* b) const;
	};
	class cstrEq
	{
	public:
		bool operator() (const char* a, const char* b) const;
	};
	struct cstrHasherTiny
	{
	public:
		size_t operator()(const CSubString& s) const;
		__inline size_t operator()(const char* s) const { return operator()(CSubString(s)); }
	};

	//static signed int findUnit(const std::string& name);
	static bool findUnit(CSubString name, int& outUnitIndex, int& outPrefixIndex);
	__inline static bool findUnit(const char* pcharName, int& outUnitIndex, int& outPrefixIndex) { return findUnit(CSubString(pcharName), outUnitIndex, outPrefixIndex); }

	static const UnitDefinition KnownUnits[];
	static const int KnownUnitsLength;
	static const Prefix KnownPrefixes[];
	static const int KnownPrefixesLength;

	// Hash table sizes are defined in hash.cpp
	static const int hashTableSize_UnitSymbols;
	static const int hashTableSize_UnitLongNames;
	static const int hashTableSize_PrefixSymbols;
	static const int hashTableSize_PrefixLongNames;

private:
	num value;
	signed char dim[(int)QuantityType::ENUM_MAX];

#ifndef NO_NEW

	//static std::unordered_map<const char*, int, cstrHasherTiny, cstrEq> UnitSymbolLookup;
	//static std::unordered_map<const char*, int, cstrHasherTiny, cstrEq> UnitLongNameLookup;
#endif
	void init();
	//void parseUnits(std::string unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut); // throws if unknown/invalid unit
	void parseUnits(const CSubString& unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut); // throws if unknown/invalid unit
	__inline void parseUnits(const char* unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut) { return parseUnits(CSubString(unitStr), unitsOut, factorOut, offsetOut); }
	void mulUnit(signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], const UnitDefinition& unit, signed int power, bool invert = false); // deals only with quantity dimension, conversion factors are handled elsewhere
};



#ifndef NO_LITERALS
// Declare literals
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
