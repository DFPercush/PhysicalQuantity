#pragma once

/*
TODO: 
	. NO_LONG_NAMES
	. NO_STRING_LOOKUPS - kills unit definition table and all text functions. Implies...
	. NO_SPRINTF, NO_SPRINTF_INT, NO_SPRINTF_FLOAT
	. MINIMAL, probably combining everything except CPP11 for constexpr
	. CPP11 --> NO_CONSTEXPR ?
*/

//=================================================================================================================
// Conditional compilation options:   (See README.txt)

#define CPP11              // Compiler supports C++11 'constexpr'
//#define NO_TYPEDEFS      // Do not define PQ:: and csubstr:: shortcuts
//#define NO_NEW           // Do not use dynamic memory allocation.
//#define NO_STD_STRING    // Do not #include <string> or use std::string .
//#define NO_LITERALS      // Do not define literal operators like 1_kg. Require a C++11 compiler or newer.
//#define NO_INLINE        // Do not use inline functions, make all functions normal calls.
//#define INLINE_KEYWORD inline  // default is __inline
//#define NO_HASHING       // Do not use hash tables for unit string lookups (Tables can be in ROM). Requires. approx. 1 KB
//#define NO_THROW         // Do not use 'throw' for errors, instead use an error callback (see errorHandler)

// End conditional compilation options
//=================================================================================================================


#ifndef MAX_NUM_TEXT_LENGTH
// The maximum length of sprintf("%g", (num))
// Danger zone! Possible buffer overflow condition if this is not tuned right.
// Allowing 26 bytes for a sprinf("%g")
// IEEE 754 doubles have a mantissa of 52 bits, exponent 11 bits.
// 16 bytes for significand = ceiling(log10(2^52))
// +1 for a decimal point
// +2 for a negative sign and negative exponent
// +4 for exponent = log10(2^11)
// +1 for null terminator
// +1 for 'e'
// +1 for a small safety margin
#define MAX_NUM_TEXT_LENGTH 26
#endif

