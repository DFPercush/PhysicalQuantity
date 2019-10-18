// cppstart: Begin PhysicalUnitDefinitions.cpp
#include <PhysicalQuantity.h>

#if !defined(NO_TEXT) || defined(PQ_GENCODE)

//const PQ::num PhysicalQuantity::KnownUnitOffsets[] = 
DEFINE_CONST_ARRAY(PhysicalQuantity::num, PhysicalQuantity::KnownUnitOffsets) = 
{
	// Any unit with an additive offset must be at the beginning
	// of KnownUnits[]
	0, // Kelvin, needs to come first for sprint() order
	273.15, // degC
	255.37222222222222222222222222222  // degF
};
const PhysicalQuantity::unitIndex_t PhysicalQuantity::KnownUnitOffsetsLength = sizeof(KnownUnitOffsets) / sizeof(PhysicalQuantity::num);
// dekaIndex: If any more prefixes are added, this should be the index of {"da", "deka", 10}
// Used to optimize lookups because this is the only prefix longer than 1 char
// If that changes, might need to change findUnit()
const PhysicalQuantity::prefixIndex_t PhysicalQuantity::dekaIndex = 10; // Index, not value, although it's fitting.
const PhysicalQuantity::prefixIndex_t PhysicalQuantity::kiloIndex = 1; // Index, not value.

#ifndef NO_LONG_NAMES
#define UN(sy, lo, pl) (sy), (lo), (pl)
#define PN(sy, lo) (sy), (lo)
//#define UN(sy, lo, pl) ROMLITERAL(sy), ROMLITERAL(lo), ROMLITERAL(pl)
//#define PN(sy, lo) ROMLITERAL(sy), ROMLITERAL(lo)
#else
#define UN(sy, lo, pl) (sy)
#define PN(sy, lo) (sy)
//#define UN(sy, lo, pl) ROMLITERAL(sy)
//#define PN(sy, lo) ROMLITERAL(sy)
#endif

const PhysicalQuantity::unitIndex_t PhysicalQuantity::gramIndex = 7;


#if defined(PQ_GENCODE) || !defined(ROM_READ_BYTE)
//const PhysicalQuantity::UnitDefinition PhysicalQuantity::KnownUnits[] = 


DEFINE_CONST_ARRAY(PhysicalQuantity::UnitDefinition, PhysicalQuantity::KnownUnits) =
{
// { symbol, longName, plural, factor, { MASS, DISTANCE, TIME, TEMPERATURE, CURRENT }, flags }
//                        Ma Di Ti Te Cu
{UN("K","kelvin","kelvin"),1.0, {0,0,0,1,0}, CANPREFIX},
{UN("degC","degreeC","degreesC"),1.0, {0,0,0,1,0}, NOPREFIX},
{UN("degF","degreeF","degreesF"),0.55555555555555555555555555555556, {0,0,0,1,0}, NOPREFIX},

{UN("rad","radian","radians"),1, { 0, 0, 0, 0, 0}, NOPREFIX},
{UN("deg","degree","degrees"),0.01745329251994329576923690768489, {0,0,0,0,0}, NOPREFIX},
{UN("pi","",""),3.1415926535897932384626433832795, {0,0,0,0,0}, NOPREFIX | NOLITERAL},
{UN("tau","",""),6.283185307179586476925286766559, {0,0,0,0,0}, NOPREFIX | NOLITERAL},
// Conflicts with fundamental charge
//{"e", "e",         { 0, 0, 0, 0, 0}, 0, 2.7182818284590452353602874713527, NO_PREFIX},

{UN("g", "gram","grams"),0.001, {1,0,0,0,0}}, // Note: Update gramIndex if this shifts!

{UN("m", "meter","meters"),1, {0,1,0,0,0}},
{UN("mi", "mile","miles"),1609.3439999931,     { 0, 1, 0, 0, 0}, NOPREFIX},
{UN("yd", "yard","yards"),0.9144, {0,1,0,0,0}, NOPREFIX},
{UN("ft", "foot","feet"),0.3048, {0,1,0,0,0}, NOPREFIX},
{UN("in", "inch","inches"),0.0254, {0,1,0,0,0}, NOPREFIX},


{UN("ang","angstrom","angstroms"),1e-10, {0,1,0,0,0}},  // unicode wchar_t is not supported at this time
//{UN("kg", "kilogram","kilograms"),1, {1,0,0,0,0}},
//                  Ma Di Ti Te Cu
{UN("s", "second","seconds"),1, {0,0,1,0,0}, NOPREFIX},
{UN("Hz", "hertz",""),1, {0,0,-1,0,0}},
{UN("min", "minute","minutes"),60, {0,0,1,0,0}, NOPREFIX},
{UN("hr", "hour","hours"),3600, {0,0,1,0,0}, NOPREFIX},

{UN("mph","MilePerHour","MilesPerHour"),1609.3439999931/3600, {0,1,-1,0,0}, NOPREFIX},

{UN("J","joule","joules"),1, {1,2,-2,0,0}},
{UN("N","newton","newtons"),1, {1,1,-2,0,0}, NOBASELITERAL},
// Conditions subject to change...
//#if defined(__GNUC__) && defined(__arm__)
//#pragma message("no base literal for N")
//NOBASELITERAL // _n and _N is a reserved suffix in some compilers
//#endif
//},

{UN("lb","pound","lbs"),4.448221615260501, {1,1,-2,0,0}, NOPREFIX},
{UN("lbs","pound","pounds"),4.448221615260501, {1,1,-2,0,0}, NOPREFIX},
//                  Ma Di Ti Te Cu

{UN("C","coulomb","coulombs"),1, {0,0,1,0,1}, NOBASELITERAL},
// Conditions subject to change...
//#if defined(__GNUC__) && defined(__arm__)
//#pragma message("no base literal for C")
//NOBASELITERAL // _C is reserved on some compilers
//#endif
//},

{UN("A","amp","amps"),1, {0,0,0,0,1}},
{UN("e","FundamentalCharge","FundamentalCharges"),1.6021765314e-19, {0,0,1,0,1}, NOPREFIX},
//{"e-","ElectronChargeNegative", {0,0,1,0,1},0, -1.6021765314e-19, NO_PREFIX},


// Coulombs <==> electron charge
// 1C = 6.24150975*10^18e or 1e = 1.60217646*10^-19C
//                   M  D Ti Te Cu
// V=J/C
// 1 ohm = 1 V/A = 1 m^2 kg s^-3 A^–2
// hp, bhp, lumens, candlesomething

};
const PhysicalQuantity::unitIndex_t PhysicalQuantity::KnownUnitsLength = sizeof(PhysicalQuantity::KnownUnits) / sizeof(PhysicalQuantity::UnitDefinition);


//const PhysicalQuantity::Prefix PhysicalQuantity::KnownPrefixes[] =
DEFINE_CONST_ARRAY(PhysicalQuantity::Prefix, PhysicalQuantity::KnownPrefixes) =
{
// Putting more common ones first
{PN("c", "centi"), 1e-2  },
{PN("k", "kilo"),  1e3   },
{PN("m", "milli"), 1e-3  },
{PN("M", "mega"),  1e6   },
{PN("u", "micro"), 1e-6  },  // Should I make the source files utf-8 for greek letters? Ehhhhh...
{PN("G", "giga"),  1e9   },
{PN("n", "nano"),  1e-9  },
{PN("T", "tera"),  1e12  },
{PN("p", "pico"),  1e-12 },
{PN("P", "peta"),  1e15  },
{PN("da", "deka"), 10.0  },
{PN("f", "femto"), 1e-15 },
{PN("d", "deci"),  0.1   },
{PN("h", "hecto"), 100.0 },
{PN("E", "exa"),   1e18  },
{PN("a", "atto"),  1e-18 },
{PN("z", "zepto"), 1e-21 },
{PN("Z", "zetta"), 1e21  },
{PN("y", "yocto"), 1e-24 },
{PN("Y", "yotta"), 1e24  }
};
const PhysicalQuantity::prefixIndex_t PhysicalQuantity::KnownPrefixesLength = sizeof(PhysicalQuantity::KnownPrefixes) / sizeof(PhysicalQuantity::Prefix);

#endif //#if defined(PQ_GENCODE) || !defined(ROM_READ_BYTE)


#endif //#if !defined(NO_TEXT) || defined(PQ_GENCODE)

// cppend: End PhysicalUnitDefinitions.cpp
