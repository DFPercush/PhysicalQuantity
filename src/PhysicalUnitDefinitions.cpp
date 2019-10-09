
#include <PhysicalQuantity.h>

const PQ::num PhysicalQuantity::KnownUnitsWithOffset[] = 
{
	// Any unit with an additive offset must be at the beginning
	// of KnownUnits[]
	0, // Kelvin, needs to come first for sprint() order
	273.15, // degC
	255.37222222222222222222222222222  // degF
};
const int PhysicalQuantity::KnownUnitsWithOffsetLength = sizeof(KnownUnitsWithOffset) / sizeof(PQ::num);


const PhysicalQuantity::UnitDefinition PhysicalQuantity::KnownUnits[] = 
{
// { symbol, longName, { MASS, DISTANCE, TIME, TEMPERATURE, CURRENT }, offset, factor, flags },
//                  Ma Di Ti Te Cu
{"K","kelvin",     { 0, 0, 0, 1, 0}, 0, 1.0, CANPREFIX},
{"degC","degreeC", { 0, 0, 0, 1, 0}, 273.15, 1.0, NOPREFIX},
{"degF","degreeF", { 0, 0, 0, 1, 0}, 255.37222222222222222222222222222, 0.55555555555555555555555555555556, NOPREFIX},


{"rad", "radian",  { 0, 0, 0, 0, 0}, 0, 1, NOPREFIX},
{"deg", "degree",  { 0, 0, 0, 0, 0}, 0, 0.01745329251994329576923690768489, NOPREFIX},
{"pi", "pi",       { 0, 0, 0, 0, 0}, 0, 3.1415926535897932384626433832795, NOPREFIX | NOLITERAL},
{"tau", "tau",     { 0, 0, 0, 0, 0}, 0, 6.283185307179586476925286766559, NOPREFIX | NOLITERAL},
// Conflicts with fundamental charge
//{"e", "e",         { 0, 0, 0, 0, 0}, 0, 2.7182818284590452353602874713527, NO_PREFIX},

{"m", "meter",     {+0,+1,+0,+0,+0}, 0, 1.0},
{"mi", "mile",     { 0, 1, 0, 0, 0}, 0, 1609.3439999931, NOPREFIX},
{"yd", "yard",     { 0, 1, 0, 0, 0}, 0, 0.9144, NOPREFIX},
{"ft", "foot",     { 0, 1, 0, 0, 0}, 0, 0.3048, NOPREFIX},
{"in", "inch",     { 0, 1, 0, 0, 0}, 0, 0.0254, NOPREFIX},


{"ang","angstrom", { 0, 1, 0, 0, 0}, 0, 1.0e-10},  // unicode wchar_t is not supported at this time
{"g", "gram",      { 1, 0, 0, 0, 0}, 0, 0.001},

{"s", "second",    { 0, 0, 1, 0, 0}, 0, 1.0, NOPREFIX},
{"Hz", "hertz",    { 0, 0,-1, 0, 0}, 0, 1.0},
{"min", "minute",  { 0, 0, 1, 0, 0}, 0, 60, NOPREFIX},
{"hr", "hour",     { 0, 0, 1, 0, 0}, 0, 3600, NOPREFIX},

{"mph","MilesPerHour",{0,1,-1,0,0}, 0, 1609.3439999931/3600, NOPREFIX},

{"J", "joule",     { 1, 2,-2, 0, 0}, 0, 1.0},
{"N", "newton",    { 1, 1,-2, 0, 0}, 0, 1.0},
{"lb", "pound",    { 1, 1,-2, 0, 0}, 0, 4.448221615260501, NOPREFIX},
//                  Ma Di Ti Te Cu

{"C","coulomb",    { 0, 0, 1, 0, 1}, 0, 1.0},
{"A","ampere",     { 0, 0, 0, 0, 1}, 0, 1.0},
{"e","FundamentalCharge", {0,0,1,0,1},0, 1.6021765314e-19, NOPREFIX},
//{"e-","ElectronChargeNegative", {0,0,1,0,1},0, -1.6021765314e-19, NO_PREFIX},


// Coulombs <==> electron charge
// 1C = 6.24150975*10^18e or 1e = 1.60217646*10^-19C
//                   M  D Ti Te Cu
// V=J/C
// 1 ohm = 1 V/A = 1 m^2 kg s^-3 A^–2


};
const int PhysicalQuantity::KnownUnitsLength = sizeof(PhysicalQuantity::KnownUnits) / sizeof(PhysicalQuantity::UnitDefinition);



const PhysicalQuantity::Prefix PhysicalQuantity::KnownPrefixes[] =
{
// Putting more common ones first
{"c", "centi", 1e-2  },
{"k", "kilo",  1e3   },
{"m", "milli", 1e-3  },
{"M", "mega",  1e6   },
{"u", "micro", 1e-6  },  // Should I make the source files utf-8 for greek letters? Ehhhhh...
{"G", "giga",  1e9   },
{"n", "nano",  1e-9  },
{"T", "tera",  1e12  },
{"p", "pico",  1e-12 },
{"P", "peta",  1e15  },
{"da", "deka", 10.0  },
{"f", "femto", 1e-15 },
{"d", "deci",  0.1   },
{"h", "hecto", 100.0 },
{"E", "exa",   1e18  },
{"a", "atto",  1e-18 },
{"z", "zepto", 1e-21 },
{"Z", "zetta", 1e21  },
{"y", "yocto", 1e-24 },
{"Y", "yotta", 1e24  }
};
const int PhysicalQuantity::KnownPrefixesLength = sizeof(PhysicalQuantity::KnownPrefixes) / sizeof(PhysicalQuantity::Prefix);

// dekaIndex: If any more prefixes are added, this should be the index of {"da", "deka", 10}
	// Used to optimize lookups because this is the only prefix longer than 1 char
	// If that changes, might need to change findUnit()
const PhysicalQuantity::prefixIndex_t PhysicalQuantity::dekaIndex = 10; // Index, not value, although it's fitting.


