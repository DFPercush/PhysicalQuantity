
#include <PhysicalQuantity.h>

const PhysicalQuantity::UnitDefinition PhysicalQuantity::KnownUnits[] = 
{
	// symbol, longName, { MASS, DISTANCE, TIME, TEMPERATURE, CHARGE }, offset, factor
	//                  Ma Di Ti Te Ch
	{"rad", "radian",   {0, 0, 0, 0, 0},0,1},
	{"deg", "degree",   {0, 0, 0, 0, 0},0, 114.59155902616464175359630962821},

{"m", "meter",     {+0,+1,+0,+0,+0}, 0, 1.0},
{"ang","angstrom", { 0, 1, 0, 0, 0}, 0, 1.0e-10},  // I really don't want to deal with unicode, sorry
{"g", "gram",      { 1, 0, 0, 0, 0}, 0, 0.001},
{"s", "second",    { 0, 0, 1, 0, 0}, 0, 1.0},
{"Hz", "hertz",    { 0, 0,-1, 0, 0}, 0, 1.0},
{"J", "joule",     { 1, 2,-2, 0, 0}, 0, 1.0},
{"N", "newton",    { 1, 1,-2, 0, 0}, 0, 1.0},
{"lb", "pound",    { 1, 1,-2, 0, 0}, 0, 4.448221615260501},
//                  Ma Di Ti Te Ch
{"K","kelvin",     { 0, 0, 0, 1, 0}, 0, 1.0},
{"degC","degreeC", { 0, 0, 0, 1, 0}, 273.15, 1.0},
{"degF","degreeF", { 0, 0, 0, 1, 0}, 255.37222222222222222222222222222, 0.55555555555555555555555555555556},
{"C","coulomb",    { 0, 0, 0, 0, 1}, 0, 1.0},
{"A","ampere",     { 0, 0,-1, 0, 1}, 0, 1.0},
{"e-","electronChargeMinus", {0,0,0,0,-1},0, -1.6021765314e-19},
{"e+","FundamentalChargePlus", {0,0,0,0,-1},0, 1.6021765314e-19},


// Coulombs <==> electron charge
// 1C = 6.24150975*10^18e or 1e = 1.60217646*10^-19C
//                   M  D Ti Te Ch


};
const int PhysicalQuantity::KnownUnitsLength = sizeof(PhysicalQuantity::KnownUnits) / sizeof(PhysicalQuantity::UnitDefinition);



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
const int PhysicalQuantity::dekaIndex = 10; // If any more prefixes are added, this should be the index of {"da", "deka", 10}
						  // Used to optimize lookups because this is the only prefix longer than 1 char
						  // If that changes, might need to change findUnit()

