#pragma once

//#define NO_THROW
//#define LOW_RAM

#ifndef NO_THROW
#include <exception>
#include <stdexcept>
#endif
#ifndef LOW_RAM
#include <vector>
#include <unordered_map>
#endif

// TODO: not to depend on std::strings
#include <string>

class substring;
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
		E_UNKNOWN = 0,
		E_UNIT_MISMATCH,
		E_INVALID_EXPRESSION
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
#endif

#ifndef LOW_RAM
#endif
	class PreferredUnitsBase
	{
		friend class PhysicalQuantity;
	protected:
		//std::vector<int> prefs;
		int* unitIndeces;
		int count_;
		//int staticStorage[4];
		//static const int staticStorageSize;
		__inline int count() { return count_; }
		const UnitDefinition& operator[] (int i);
	};

#ifndef LOW_RAM
	class PreferredUnits_dynamic : public PreferredUnitsBase
	{
	public:
		PreferredUnits_dynamic(const std::string& unitList_SpaceDelimited);
		~PreferredUnits_dynamic();
	};
#endif
	class PreferredUnits_static : public PreferredUnitsBase
	{
	public:
		PreferredUnits_static(const char* unitList, int* storage, size_t storageSizeBytes);
	};
#ifdef LOW_RAM
	typedef PreferredUnits_static PreferredUnits;
#else
	typedef PreferredUnits_dynamic PreferredUnits;
#endif
	
	static const num Pi;

	PhysicalQuantity();
	PhysicalQuantity(const PhysicalQuantity& copy);
	PhysicalQuantity(PhysicalQuantity&& move) noexcept;
	PhysicalQuantity(num value, const std::string& unit);
	PhysicalQuantity(const std::string& str);
	//PhysicalQuantity(num value, const std::vector<UnitQty>& units);
	//PhysicalQuantity(num value, std::vector<UnitQty>&& units);
	~PhysicalQuantity() = default;

	// To read the value, specify the unit, must match
	//num convert(const std::vector<UnitQty> unit);
	num convert(const std::string& units);

	//void parse(const char* text);
	void parse(std::string text);
	//__inline void parse(std::string text) { parse(text.c_str()); }
	std::string toString();
	std::string toString(const PreferredUnitsBase&);

	PhysicalQuantity operator* (const PhysicalQuantity& rhs);
	PhysicalQuantity operator/ (const PhysicalQuantity& rhs);
	PhysicalQuantity operator+ (const PhysicalQuantity& rhs);
	PhysicalQuantity operator- (const PhysicalQuantity& rhs);

	PhysicalQuantity operator* (num rhs);
	PhysicalQuantity operator/ (num rhs);
	PhysicalQuantity operator+ (num rhs);
	PhysicalQuantity operator- (num rhs);

	bool operator==(const PhysicalQuantity& rhs);

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
		size_t operator()(const char*) const;
	};

	static signed int findUnit(const std::string& name);
	static bool findUnit(const char* name, int& outUnitIndex, int& outPrefixIndex);

	static const UnitDefinition KnownUnits[];
	static const int KnownUnitsLength;
	static const Prefix KnownPrefixes[];
	static const int KnownPrefixesLength;

private:
	num value;
	signed char dim[(int)QuantityType::ENUM_MAX];

#ifndef LOW_RAM

	static std::unordered_map<const char*, int, cstrHasherTiny, cstrEq> UnitSymbolLookup;
	static std::unordered_map<const char*, int, cstrHasherTiny, cstrEq> UnitLongNameLookup;
#endif
	void init();
	void parseUnits(std::string unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], num& factorOut, num& offsetOut); // throws if unknown/invalid unit
	void mulUnit(signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], const UnitDefinition& unit, signed int power, bool invert = false); // deals only with quantity dimension, conversion factors are handled elsewhere
};

class CSubString
{
private:
	const char* str;
	int start;
	int end; // length in constructor, internally end is easier
public:
	CSubString(const char* str_arg, size_t start_arg, size_t len_arg);
	bool operator== (const char* cmp);
	bool begins(const char* test);
	bool ends(const char* test);
	int at(const char* test, int start = 0);
	__inline int size() { return end - start; }
	__inline int length() { return end - start; }
	bool copy(char* buf, int size);
};

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
