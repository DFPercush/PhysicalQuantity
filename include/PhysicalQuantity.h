#pragma once

#include <vector>
#include <exception>
#include <stdexcept>
#ifndef LOW_RAM
#include <unordered_map>
#endif
#include <string>

class PhysicalQuantity
{
public:
	
	enum class QuantityType // : int
	{
		MASS,
		DISTANCE,
		TIME,
		TEMPERATURE,
		CHARGE,
		ENUM_MAX
	};

	//struct QtyPower
	//{
	//	QuantityType type;
	//	signed int power;
	//};


	struct UnitQty
	{
		//UnitType type;
		QuantityType type;
		//UnitDefinition unit;
		//int unitDefIndex;
		signed int power;
	};


	struct UnitDefinition
	{
		const char* symbol;
		const char* longName;
		//const UnitQty* qtys;
		//unsigned int nqtys;
		const signed char dim[(int)QuantityType::ENUM_MAX];
		double offset; // temperature is a special snowflake
		double factor;
	};

	struct Prefix
	{
		const char* symbol;
		const char* longName;
		double factor;
	};

	//enum class UnitType
	//{
	//	UNKNOWN = 0,
	//
	//	METER,
	//
	//	KILOGRAM,
	//
	//	SECOND,
	//
	//	DEGREE_K,
	//	DEGREE_C,
	//	DEGREE_F,
	//
	//	COULOMB,
	//
	//	HENRY,
	//
	//	TESLA,
	//
	//	VOLT,
	//
	//	AMPERE,
	//
	//	OHM
	//};

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

	class PreferredUnits
	{
	public:
		PreferredUnits(const std::string& unitList_SpaceDelimited);
	private:
		std::vector<int> prefs;
	};

	static const double Pi;

	PhysicalQuantity();
	PhysicalQuantity(const PhysicalQuantity& copy);
	PhysicalQuantity(PhysicalQuantity&& move) noexcept;
	PhysicalQuantity(double value, const std::string& unit);
	PhysicalQuantity(const std::string& str);
	//PhysicalQuantity(double value, const std::vector<UnitQty>& units);
	//PhysicalQuantity(double value, std::vector<UnitQty>&& units);
	~PhysicalQuantity() = default;

	// To read the value, specify the unit, must match
	//double convert(const std::vector<UnitQty> unit);
	double convert(const std::string& units);

	//void parse(const char* text);
	void parse(std::string text);
	//__inline void parse(std::string text) { parse(text.c_str()); }
	std::string toString();

	PhysicalQuantity operator* (const PhysicalQuantity& rhs);
	PhysicalQuantity operator/ (const PhysicalQuantity& rhs);
	PhysicalQuantity operator+ (const PhysicalQuantity& rhs);
	PhysicalQuantity operator- (const PhysicalQuantity& rhs);

	PhysicalQuantity operator* (double rhs);
	PhysicalQuantity operator/ (double rhs);
	PhysicalQuantity operator+ (double rhs);
	PhysicalQuantity operator- (double rhs);


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
private:
	double value;
	signed char dim[(int)QuantityType::ENUM_MAX];
	//std::vector<UnitQty> units;
	//std::vector<int> friendlyUnits;

	static const UnitDefinition KnownUnits[];
	static const int KnownUnitsLength;
	static const Prefix KnownPrefixes[];
	static const int KnownPrefixesLength;
#ifndef LOW_RAM

	static std::unordered_map<const char*, int, cstrHasherTiny, cstrEq> UnitSymbolLookup;
	static std::unordered_map<const char*, int, cstrHasherTiny, cstrEq> UnitLongNameLookup;
	//static std::map<const char*, int, cstrLess> UnitLongNameLookup;
#endif
	void init();
	signed int findUnit(const std::string& name);
	void parseUnits(std::string unitStr, signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], double& factorOut); // throws if unknown/invalid unit
	void mulUnit(signed char (&unitsOut)[(int)QuantityType::ENUM_MAX], const UnitDefinition& unit, signed int power, bool invert = false); // deals only with quantity dimension, conversion factors are handled elsewhere
};

// Declare literals
#define DeclareLiteral(symbol) PhysicalQuantity operator ""_##symbol(long double); PhysicalQuantity operator ""_##symbol(unsigned long long);

DeclareLiteral(m);
DeclareLiteral(g);
DeclareLiteral(s);
DeclareLiteral(Hz);
DeclareLiteral(N);
DeclareLiteral(J);
