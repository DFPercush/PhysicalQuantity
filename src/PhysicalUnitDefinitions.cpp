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



#if defined(PQ_GENCODE) || !defined(ROM_READ_BYTE)
//const PhysicalQuantity::UnitDefinition PhysicalQuantity::KnownUnits[] = 

const PhysicalQuantity::unitIndex_t PhysicalQuantity::gramIndex = 3;

DEFINE_CONST_ARRAY(PhysicalQuantity::UnitDefinition, PhysicalQuantity::KnownUnits) =
{
//===============================================================================================
// Avoid inserting anything between here and the next comment like this.

// { symbol, longName, plural, factor, { MASS, DISTANCE, TIME, TEMPERATURE, CURRENT }, flags }
//                        Ma Di Ti Te Cu An
{UN("K","kelvin","kelvin"),1.0, {0,0,0,1,0}, CANPREFIX},
{UN("degC","degreeC","degreesC"),1.0, {0,0,0,1,0}, NOPREFIX},
{UN("degF","degreeF","degreesF"),0.55555555555555555555555555555556, {0,0,0,1,0}, NOPREFIX},

// Mass
{UN("g", "gram","grams"),0.001, {1,0,0,0,0}}, // Note: Update gramIndex if this shifts!

// Avoid inserting anything above here. There are other constants which index this array.
// Temperature degrees always need to come first.
//===============================================================================================

{UN("amu", "atomic_mass_unit","atomic_mass_units"),0.000000000000000000000000001660539, {1,0,0,0,0}, NOPREFIX},
{UN("carat", "carat","carats"),0.0002, {1,0,0,0,0}, NOPREFIX},
{UN("dram", "dram","drams"),0.001771845, {1,0,0,0,0}, NOPREFIX},
{UN("hundredweight", "hundredweight","hundredweights"),45.35924, {1,0,0,0,0}, NOPREFIX},
{UN("longton", "longton","longtons"),1016.05, {1,0,0,0,0}, NOPREFIX},
{UN("long_ton", "long_ton","long_tons"),1016.05, {1,0,0,0,0}, NOPREFIX},
{UN("t", "metric_ton","metric_tons"),1000, {1,0,0,0,0}, NOPREFIX},
{UN("oz", "ounce","ounces"),0.02834952, {1,0,0,0,0}, NOPREFIX},
{UN("lbs_mass", "pound_mass","lbs_mass"),0.4535924, {1,0,0,0,0}, NOPREFIX},
{UN("lb_mass", "pound_mass","pounds_mass"),0.4535924, {1,0,0,0,0}, NOPREFIX},
{UN("lb", "pound","pounds"),0.4535924, {1,0,0,0,0}, NOPREFIX},
{UN("lbs", "pound","pounds"),0.4535924, {1,0,0,0,0}, NOPREFIX},
{UN("shortton", "shortton","shorttons"),907.185, {1,0,0,0,0}, NOPREFIX},
{UN("short_ton", "short_ton","short_tons"),907.185, {1,0,0,0,0}, NOPREFIX},
{UN("slug", "slug","slugs"),14.5939, {1,0,0,0,0}, NOPREFIX},
{UN("stick", "stick","sticks"),0.115, {1,0,0,0,0}, NOPREFIX},
{UN("stone", "stone","stones"),6.350293, {1,0,0,0,0}, NOPREFIX},
{UN("tola", "tola","tolas"),0.0116638, {1,0,0,0,0}, NOPREFIX},
{UN("ton", "ton","tons"),907.1847, {1,0,0,0,0}, NOPREFIX},
{UN("ozt", "troy_ounce","troy_ounces"),0.03110348, {1,0,0,0,0}, NOPREFIX},
{UN("dwt", "pennyweight","pennyweights"),0.03110348/20, {1,0,0,0,0}, NOPREFIX},
//{UN("grain", "grain","grains"),0.03110348/20/24, {1,0,0,0,0}},
{UN("grain", "grain","grains"),0.00006479891, {1,0,0,0,0}, NOPREFIX}, // Same as prev line
{UN("lbt", "troy_pound","troy_pounds"),0.03110348*12, {1,0,0,0,0}, NOPREFIX},
{UN("lbst", "troy_pound","troy_pounds"),0.03110348*12, {1,0,0,0,0}, NOPREFIX},

// Angles
// "rad" would conflict with rads of radiation
// Leaving the symbol blank on the first radian definition, because it should not be output for 
// things like "1 ft lb_force * 1 rpm == 1 hp" not "1 hp radians"
// but users can still manually specify it, and there are multiple entries here
// to allow for literals like 1_radian. Otherwise 3.14 would just be a number, not an angle.
// Which, honestly, is fine if you don't need the conversion, but having it flagged as
// an actual angle might make things more clear.
{UN("","radian","radians"),1, {0,0,0,0,0,1}, NOPREFIX},
{UN("radians","",""),1, {0,0,0,0,0,1}, NOPREFIX}, // for the literal
{UN("radian","",""),1, {0,0,0,0,0,1}, NOPREFIX}, // for the literal
{UN("deg","degree","degrees"),0.01745329251994329576923690768489, {0,0,0,0,0,1}, NOPREFIX},
{UN("grad","gradian","gradians"),0.0157079632679489661923132169164, {0,0,0,0,0,1}, NOPREFIX},
{UN("rev","revolution","revolutions"),6.283185307179586476925286766559, {0,0,0,0,0,1}, NOPREFIX},

// Angle composites
{UN("rpm","revolution_per_minute","revolutions_per_minute"),6.283185307179586476925286766559 / 60.0,{0,0,-1,0,0,1,0}, NOPREFIX},

// Debatable whether these should be included.
// Not really units, just numbers, named constants
{UN("pi","",""),3.1415926535897932384626433832795, {0,0,0,0,0}, NOPREFIX | NOLITERAL},
{UN("tau","",""),6.283185307179586476925286766559, {0,0,0,0,0}, NOPREFIX | NOLITERAL},

// Other scalars


// Conflicts with fundamental charge
//{"e", "e",         { 0, 0, 0, 0, 0}, 0, 2.7182818284590452353602874713527, NO_PREFIX},

// Distance (Length)
{UN("m", "meter","meters"),1, {0,1,0,0,0}},
{UN("mi", "mile","miles"),1609.3439999931,     { 0, 1, 0, 0, 0}, NOPREFIX},
{UN("yd", "yard","yards"),0.9144, {0,1,0,0,0}, NOPREFIX},
{UN("ft", "foot","feet"),0.3048, {0,1,0,0,0}, NOPREFIX},
{UN("in", "inch","inches"),0.0254, {0,1,0,0,0}, NOPREFIX},
{UN("ang","angstrom","angstroms"),1e-10, {0,1,0,0,0}},  // unicode wchar_t is not supported at this time
{UN("AU","astronomical_unit","astronomical_units"), 149597900000, {0,1,0,0,0}},
{UN("chain","chain","chains"),20.11680394008, {0,1,0,0,0}, NOPREFIX},
{UN("ly","light_year","light_years"),9460732325561360, {0,1,0,0,0}},
{UN("li","link","links"),0.201168039400805, {0,1,0,0,0}, NOPREFIX},
{UN("mil","mil","mils"),0.0000254, {0,1,0,0,0}, NOPREFIX}, // thousandth of an inch
{UN("thou","thou","thous"),0.0000254, {0,1,0,0,0}, NOPREFIX}, // thousandth of an inch
{UN("nmi","nautical_mile","nautical_miles"),1852, {0,1,0,0,0}, NOPREFIX},
{UN("pc","parsec","parsecs"),30856778900000000, {0,1,0,0,0}},
{UN("rod","rod","rods"), 5.0292105028246334, {0,1,0,0,0}, NOPREFIX},


//Area
{UN("b", "barn","barns"), 1e-28, {0,2,0,0,0}},
{UN("a", "are","ares"), 100, {0,2,0,0,0}, NOPREFIX},
{UN("ha", "hectare","hectares"), 10000, {0,2,0,0,0}, NOPREFIX},
{UN("ac", "acre","acres"), 4046.86, {0,2,0,0,0}, NOPREFIX},
{UN("ro", "rood","roods"), 1011.714, {0,2,0,0,0}, NOPREFIX},


// Volume
{UN("L", "liter","liters"), 0.001, {0,3,0,0,0}},
{UN("cc", "cubic_centimeter","cubic_centimeters"), 0.000001, {0,3,0,0,0}, NOPREFIX},
{UN("gal", "gallon","gallons"), .003785411784, {0,3,0,0,0}, NOPREFIX},
{UN("qt", "quart","quarts"), 0.000946352946, {0,3,0,0,0}, NOPREFIX},
{UN("pt", "pint","pints"), 0.000473176473, {0,3,0,0,0}, NOPREFIX},
{UN("cu", "cup","cups"), 0.0002365882365, {0,3,0,0,0}, NOPREFIX},
{UN("floz", "fluid_ounce","fluid_ounces"), 0.0000295735295625, {0,3,0,0,0}, NOPREFIX},
{UN("tbsp", "tablespoon","tablespoons"), 0.00001478676478125, {0,3,0,0,0}, NOPREFIX},
{UN("tsp", "teaspoon","teaspoons"), 0.00000492892159375, {0,3,0,0,0}, NOPREFIX},
{UN("beerbarrel", "beerbarrel","beerbarrels"), .1173478, {0,3,0,0,0}, NOPREFIX},
{UN("drop", "drop","drops"), 0.00000005, {0,3,0,0,0}, NOPREFIX},
{UN("fldram", "fluid_dram","fluid_drams"), .000003696691, {0,3,0,0,0}, NOPREFIX},
{UN("gill", "gill","gills"), .0001182941, {0,3,0,0,0}, NOPREFIX},
{UN("hogshead", "hogshead","hogsheads"), .238481, {0,3,0,0,0}, NOPREFIX},
{UN("minim", "minim","minims"), 0.00000006161152, {0,3,0,0,0}, NOPREFIX},
{UN("oilbarrell", "oilbarrel","oilbarrels"), .1589873, {0,3,0,0,0}, NOPREFIX},



//                  Ma Di Ti Te Cu An

// Time
{UN("s", "second","seconds"),1, {0,0,1,0,0}}, //, NOPREFIX},
{UN("Hz", "hertz",""),1, {0,0,-1,0,0}},
{UN("min", "minute","minutes"),60, {0,0,1,0,0}, NOPREFIX},
{UN("hr", "hour","hours"),3600, {0,0,1,0,0}, NOPREFIX},
{UN("h", "hour","hours"),3600, {0,0,1,0,0}, NOPREFIX},

// Speed
{UN("mph","mile_per_hour","miles_per_hour"),1609.3439999931/3600.0, {0,1,-1,0,0}, NOPREFIX},
{UN("kt","knot","knots"),0.51444444444444444444444444444444, {0,1,-1,0,0}, NOPREFIX},

// Acceleration
{UN("g0","gee","gees"),9.80665, {0,1,-2,0,0}, NOPREFIX},


// Energy
{UN("J","joule","joules"),1, {1,2,-2,0,0}},
{UN("erg","erg","ergs"),1e-7, {1,2,-2,0,0}},
{UN("Wh","watt-hour","watt-hours"),3600, {1,2,-2,0,0}},
{UN("eV","electron_volt","electron_volts"),1.602176634e-19, {1,2,-2,0,0}},

//                        Ma Di Ti Te Cu An
// Power
{"W", "watt", "watts", 1, {1,2,-3,0,0}},
{"hp", "horsepower","", 745.699872, {1,2,-3,0,0}},
{"hp_electric", "electrichorsepower", "", 746, {1,2,-3,0,0}},
{"hp_metric", "metrichorsepower", "", 735.49875, {1,2,-3,0,0}},


// Force
{UN("N","newton","newtons"),1, {1,1,-2,0,0}}, //, NOBASELITERAL},
{UN("lbs_force","pound_force","pounds_force"),4.448221615260501, {1,1,-2,0,0}, NOPREFIX},
{UN("lb_force","pound_force","lbs_force"),4.448221615260501, {1,1,-2,0,0}, NOPREFIX},



// Conditions subject to change...
//#if defined(__GNUC__) && defined(__arm__)
//#pragma message("no base literal for N")
//NOBASELITERAL // _n and _N is a reserved suffix in some compilers
//#endif
//},
//                  Ma Di Ti Te Cu An

// Conditions subject to change...
//#if defined(__GNUC__) && defined(__arm__)
//#pragma message("no base literal for C")
//NOBASELITERAL // _C is reserved on some compilers
//#endif
//},

// Current and charge
{UN("A","amp","amps"),1, {0,0,0,0,1}},
{UN("","ampere","amperes"),1, {0,0,0,0,1}},
{UN("C","coulomb","coulombs"),1, {0,0,1,0,1}}, //, NOBASELITERAL},
{UN("e","FundamentalCharge","FundamentalCharges"),1.6021765314e-19, {0,0,1,0,1}, NOPREFIX},
//{"e-","ElectronChargeNegative", {0,0,1,0,1},0, -1.6021765314e-19, NO_PREFIX},

//                           Ma Di Ti Te Cu An
// Composite electromagnetic units
{UN("V", "volt", "volts"), 1, {1,2,-3,0,-1}},
{UN("ohm", "ohm", "ohms"), 1, {1,2,-3,0,-2}}, // TODO: Test unicode support? I think UTF-8 would still work with strcmp() for equality testing
{UN("S", "siemens", ""), 1, {-1,-2,3,0,2}}, // conductance
{UN("F", "farad", "farads"), 1, {-1,-2,4,0,2}}, // capacitance
{UN("Wb", "weber", "webers"), 1, {1,2,-2,0,-1}},
{UN("T", "tesla", "teslas"), 1, {1,0,-2,0,-1}}, // some of these plurals might not be accurate, but will be permissive of user error when we know what they're talking about
{UN("H", "henry", "henrys"), 1, {1,2,-2,0,-1}},

//                      Ma Di Ti Te Cu An
// Radiation
{UN("ct", "count", "counts"), 1, {0,0,0,0,0,0}},
{UN("cpm", "count_per_minute", "counts_per_minute"), 1.0/60.0, {0,0,-1,0,0,0}},
{UN("Bq", "becquerel", "becquerels"), 1, {0,0,-1,0,0}}, // radionuclide activity, counts per second
{UN("Rd", "rutherford", "rutherford"), 1e6, {0,0,-1,0,0}}, // radionuclide activity, counts per second
{UN("Gy", "gray", "grays"), 1, {0,2,-2,0,0}},  // dose, J/kg
{UN("rad", "rad", "rads"), 0.01, {0,2,-2,0,0}},  // dose
{UN("R", "roentgen", "roentgens"),2.58e-4, {-1,0,1,0,1}},  // dose

// Because sieverts involve multiplying by a factor depending on biology, there's not a good way to represent that
//{UN("Sv", "sievert", "sieverts"), 1, {0,2,-2,0,0}}, // equivalent dose, involves a scalar factor

//TODO: kat, katal, s^-1*mol, catalytic activity


// TODO: Light: candela, lumens etc
// dyne, kip, sthene

//                      Ma Di Ti Te Cu An
// Pressure
{UN("Pa", "pascal","pascals"), 1,{1,-1,-2,0,0}},
{UN("bar", "bar","bars"), 100000,{1,-1,-2,0,0}},
{UN("psi", "",""), 6894.757293178,{1,-1,-2,0,0}},
{UN("torr", "Torr",""), 133.3223684211,{1,-1,-2,0,0}},
{UN("mmHg", "",""), 133.322,{1,-1,-2,0,0}},
{UN("cmHg", "",""), 1333.22,{1,-1,-2,0,0}},
{UN("atm", "atmosphere","atmospheres"), 101325,{1,-1,-2,0,0}},
{UN("Ba", "bary","barye"), 0.1,{1,-1,-2,0,0}},
{UN("", "pièze","pieze"), 1000,{1,-1,-2,0,0}},
//{UN("", "",""), ,{1,-1,-2,0,0}},

// 

};
const PhysicalQuantity::unitIndex_t PhysicalQuantity::KnownUnitsLength = sizeof(PhysicalQuantity::KnownUnits) / sizeof(PhysicalQuantity::UnitDefinition);

//const PhysicalQuantity::Prefix PhysicalQuantity::KnownPrefixes[] =
DEFINE_CONST_ARRAY(PhysicalQuantity::Prefix, PhysicalQuantity::KnownPrefixes) =
{
// Putting more common ones first
{PN("", ""), 1 },
{PN("c", "centi"), 1e-2  },
{PN("k", "kilo"),  1e3   },
{PN("m", "milli"), 1e-3  },
{PN("M", "mega"),  1e6   },
{PN("u", "micro"), 1e-6  },  // Should I make the source files utf-8 for greek letters? Would require re-coding a lot to handle wchar_t... meh
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


// dekaIndex: If any more prefixes are added, this should be the index of {"da", "deka", 10}
// Used to optimize lookups because this is the only prefix longer than 1 char
// If that changes, might need to change findUnit()
const PhysicalQuantity::prefixIndex_t PhysicalQuantity::dekaIndex = 11; // Index, not value
const PhysicalQuantity::prefixIndex_t PhysicalQuantity::kiloIndex = 2; // Index, not value.

#endif //#if !defined(NO_TEXT) || defined(PQ_GENCODE)

// cppend: End PhysicalUnitDefinitions.cpp
