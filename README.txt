PhysicalQuantity
Original version by Daniel Papenburg, 2019
github.com/DFPercush
free/open source, just don't plagiarize


	PhysicalQuantity is an embedded-friendly C++ class library which
allows direct arithmetic operations on values including the basic units of
distance, mass, time, temperature, and charge (or current), as well as
any other units derived from these quantities. This allows different teams 
working on a project to operate in their preferred units without things going boom.
	It keeps track of the dimensions of a value during all calculations, and 
handles multiplication and division of units, conversion factors, and dimension checking
during addition and subtraction, as well as scalar operations.  It includes a
.eval() function for parsing whole math expressions* as well as .sprint() for 
generating text output, and .convert() for extraction of numeric values in code.
Test console included. 
	You also get CSubString, a non-allocating c-style char* string parsing
helper class for free!

* whitespace between values is important!
	Bad: 2km
	Good: 2 km
	Bad: 1+2
	Good: 1 + 2
	Bad: (1 m+2 km)/s
	Good: (1 m + 2 km) / s
		Note: You can add different units as long as they are like quantities.
	"3 m^2"   !=   ("3 m ^ 2" == "(3 m) ^ 2" == "9 m^2")
	"5 * m^2" == "5 m^2"
	... for now anyway.



===================================================================================================
System requirements:

	- A C++ compiler. Preferably C++11 or newer, but older will work with some config.

	- log10()

	- 

	- approx. 1KB of ROM space for hash tables. This speeds up parsing and output.
		#define NO_HASHING to disable.

	- eval() is recursive, stack usage depends on input.

What is NOT required:
	- Templates
	- STL
	- new/delete/malloc/free
	- std::string
	- try/catch

ppOptions.h contains some conditional compilation options for limited systems:
	CPP11            // Compiler supports C++11
	NO_NEW           // Do not use dynamic memory allocation.
	NO_STD_STRING    // Do not #include <string> or use std::string. Implied by NO_NEW.
	NO_LITERALS      // Do not define literal operators like 1_kg. Require a C++11 compiler or newer.
	NO_INLINE        // Do not use inline functions, make all functions normal calls.
	INLINE_KEYWORD inline  // default is __inline
	NO_HASHING       // Do not use hash tables for unit string lookups (Tables can be in ROM).
	NO_THROW         // Do not 'throw' errors, instead use an error callback (see errorHandler)
	NO_NETWORK       // Do not include read/writeNetworkBinary functions, depends on socket header

	TODO: NO_CONSTEXPR, NO_SPRINTF, NO_SPRINTF_INT, NO_SPRINTF_FLOAT



===================================================================================================
Installation/setup:
	If all you want to do is build the library and use the test console, MSBuild / F7
or 'make' should get you a binary without much effort. 
	For Visual Studio builds, msvc/RootDevDir.props contains the path where you
put most new projects. If you always put your projects in the default path of
C:\Users\you\Source\Repos, you can put that path in this property file to make it easier
to include PhysicalQuantity in other projects later.
Using a text editor to modify RootDevDir.props might be easier than trying to
navigate visual studio's property pages, but it will be in the user macros section.
 These property pages exist so that you can start a new
project in another directory, import these settings, and have your include and library
paths all set and ready to go. So basically you can do this:
#include <PhysicalQuantity.h>
... instead of this
#include "../../somewhere/only/you/put/stuff/PhysicalQuantity.h"
... which can save headaches when working on shared repositories.

	For embedded use, you will need to build and run the 'genhashtables' binary on a 
PC first, before you try to cross compile for the target system, because the
unit lookup hash tables are not included in the repo, for space reasons.
Does not apply if using NO_HASHING. Users who plan on using this code on a
PC will still have to do this, but it's set up as part of the build process.
The output of genhashtables should be copied or piped to
	include\PhysicalQuantity\hashTables.h

	If you think you might be creating new commits and pull requests,
you will need to tell git not to track changes to certain config files. 
Please run the following commands:

	git update-index --skip-worktree msvc/RootDevDir.props
	git update-index --skip-worktree include/PhysicalQuantity/ppOptions.h

	Likewise for any other files you change that reflect only your local setup.

	If you need to change project configuration, it's a good idea to create a new
property sheet in visual studio and put the settings there, so you're not committing
your personal settings in the shared git repo.




===================================================================================================
Conditional compilation

	For any of these #define options, you can either uncomment the related line
in ppOptions.h, you can #define the option in your source file before #including 
the header, or you can pass the #define as an argument to the compiler. All this 
flexibility does makes it necessary, however, to check that various modules are 
compiled with the same configuration as their dependencies. There is an easy macro 
for this purpose called 'PQHeaderOptionsMatch'.  Here's an example:

	#include <PhysicalQuantity.h>
	int main() {
		if (!PQHeaderOptionsMatch) {
			printf("Error: library code was compiled with different header options.\n");
			return 1;
		}
		...
	}

	This check is performed every time you try to instantiate a PhysicalQuantity 
object. If the options do not match, an error will be generated. The overhead for
this operation is a simple comparison between a literal/immediate value and a
static const stored at a particular address, probably in ROM if you have it.


===================================================================================================
Performance:
	Internal math operations are very fast (as long as you have floating point
hardware), but when you want to parse a value in, or print one out, some lookups
are necessary to convert between units. A lot of the preprocessor options deal
with how this particular aspect is handled. You can have a hash table in ROM to
make unit lookups faster, or use a slower linear search for minimum memory 
overhead. There is also a cache mechanism for storing parsed unit strings:
See PhysicalQuantity::UnitList
	Performing arithmetic with these objects, once parsed, basically
involves one floating point operation and 5 integer additions/comparisons.
In the case of eval()'ing an exponentiation, that's 5 integer multiplies and a pow().
But eval() has its own overhead anyway. There's no built in exponent operator,
but you can use sprint() + eval() to accomplish this. A word of caution, though:
values with units can only be raised to integer exponents. 1 meter ^ 2.3  will
throw an error.


Q: Can a PhysicalQuantity be binary-copied with memcpy() or DMA?
A: Yes.

Q: Can it be sent over a network in binary?
A: I wrote a long answer for this, then I wrote readNetworkBinary() and
   writeNetworkBinary(). IP uses big endian numbers basically.

Q: Does the structure store references or pointers to any external buffers?
A: Not the main PhysicalQuantity type. No pointers there.
   But the UnitList class which is used for formatting text output 
   requires some buffer storage. In static mode (NO_NEW), you will need
   to supply this buffer. You can choose to explicitly use UnitList_static
   any time, but UnitList_dynamic is only available #ifndef NO_NEW.
   UnitList is a typedef that conditionally aliases to either the static
   or dynamic version.


===================================================================================================
Memory usage:
	Aside from the size of the binary generated by your compiler which can vary,
the data does not take much memory at all. The precision of values is governed by
the typedef 'num' in the main header file. By default, this is 'double'.
Each value currently requires space for a 'num' plus 5 bytes,
so a total of 13 bytes per value, out of the box. Changing the 'num' typedef
to 'float' would bring that down to 9.
	In reality, the addition of padding bytes will probably mean that for double
precision, the size will be 16 bytes, and for float, 12.
	Theoretically, it might be possible to modify the unit storage as a struct of
bit fields to perhaps 2 bytes instead of 5. Not often does one run into units 
of the 127th power, but that is possible in the default configuration.
Bit fields would make calculations slightly slower, requiring an extra 
fetch/shift/or operation on every read/write, and speed is also
important. But if you really need to save space, it might be worth looking into.
That is, if your compiler supports it. This would involve modifying code, a little
more involved than a simple preprocessor option. There are a lot of places where
memcmp() is used to check these bytes, and that might not work so well if there are
extra undefined bits. I didn't see the need to cram bits like that, but
that's how I would go about it if I had to.
	STL containers are strictly avoided, and dynamic memory allocation can be turned 
off with NO_NEW. The main place memory buffers are required is the UnitList
class. It can operate in dynamic or static mode. UnitList 
is how you specify the units of text output. It is basically a cache of unit 
lookups which you can reuse in subsequent toString() calls, rather than having
to parse the units each time.
	In the static case, you need to give UnitList 
a storage buffer of (int[however many units you tell it * 2]) when constructed. 
Note that the size parameter is specified in bytes, not elements.
A buffer which is too small will not generate an error, but it may cause 
toString() output to show the wrong units. None of this is required if you
just want to get the value, though.
	For any code in which you need to read and work with the numeric value,
rather than displaying or logging it, you should use convert() which does
not have any buffer storage requirements. convert(), however, is more strict
in that you must request a value of a specific unit, and if the dimensions of
that unit do not match the stored value, an error occurs.
	For example, if you perform a calculation of energy in terms of Joules,
you can convert("kWh") just fine, because kWh is another unit of energy.
But trying to convert("cm") will produce an error.


A note about vectors:
	You may decide to code a vector of PhysicalQuantity's, but the class itself can not
represent a vector. It represents a single numeric value and its associated units.
You can, however, compare the dimensions of two values to see if they represent the
same kind of quantity. a.unitsMatch(b), alias a.isLikeQuantity(b)
	Any template code which performs vector math on a template type should work with
PQ objects, as most of the basic arithmetic operators are implemented. 
Exceptions are modulo (%), increment (++) and decrement (--).
Bitwise and logical operators are not supported, but boolean comparisons like > and <= are,
with the caveat that the values must have like dimensions (same kind of units) or
an error will be thrown/called.


A note about angles:
	Since angles are dimensionless ratios (length/length), they are stored internally as a
simple scalar value. However, one can convert() this internal value to and from degrees 
or radian units. You may use the literal operators _deg and _rad to specify
hard coded values, as well as the nominal units of "deg" and "rad" when parsing / 
printing / convert()ing, to apply the proper conversion factor.
Currently there is no method of internally differentiating an angle from any other 
scalar value. Most quantities which are ratios of like units will exhibit this property.
(TODO: angle flag?)


A note about temperature:
	Temperature is a special snowflake. Degrees, whether Celsius or Fahrenheit, not
only involve a conversion factor, but also a conversion offset. In other words, an
amount that must be added to the value as well as a multiplication factor. The unique
nature of these units means that certain otherwise parseable values, such as
"16 degF^2" will produce an error, never mind that the square of a temperature
doesn't really make sense. Be careful that any units with an offset (degF and degC)
only have a power of 1 or -1 when parsing. Additionally, if the value contains any
other dimensions, such as degrees C/F per second, the offset will not be applied and 
the internal value will represent the delta-T in Kelvin / sec.
	If any of you science types out there think this is a bad idea, feel free to 
contact me or comment on github.




Exception safety:
	If an error occurs during an operation such as adding two dissimilar values,
the original value (left hand side) will remain unmodified. If a parsing error
occurs during the construction of an object, its value will be a scalar 0.
Errors during toString/sprint/convert will not modify the value, but you may not 
get the correct / any output.
	

Error handling:
	The class can throw a few exceptions, all based on std::runtime_error. Here's what they mean:
		- UnitMismatchException - Units do not match during add, subtract, or convert()
		- InvalidExpressionException - There was a parsing error.
		- HeaderConfigException - The #define options are different between your code and
		  what the library / object was compiled with.
		- std::overflow - unit exponent too large
		- std::logic error - Congratulations, you found a bug.

	If NO_THROW is #defined, the class will declare a couple of static members which
are publicly accessible to you: 'errorHandler', which is a pointer to a function,
and 'errorUserContext', which is an opaque void* pointer for you to use however
you need to. During your system init phase, you should definitely set these to
something. Here's some recommended code:
	typedef PhysicalQuantity PQ;
	PQ::ErrorCode g_pqErr;
	void pqErrorHandler(void* context, PQ::ErrorCode e) {
		g_pqErr = e;
		flashWarningLightsOrSomething();
	}
	int main() {
		PhysicalQuantity::errorHandler = myErrorHandler;
		PhysicalQuantity::errorUserContext = nullptr;
		...
	}
	int DeepInTheBowelsOfYourCode() {
		PQ makeAnError = 1.2_km + 3.4_g;
		if (g_pqErr) 
		{
			handleSomehow(); 
			g_pqErr = 0; 
			return -1; 
		}
		...
		return 0;
	}

	Instead of defining types to throw as exception objects, there is now 
an 'enum ErrorCode', which includes E_SUCCESS = 0 for convenience.
How exactly you implement error handling from this point is up to you.
You may want to pass some process info as your context, and perform specific
cleanup actions related to it in case of an error. You can also use local
variables to push the state of errorHandler and errorUserContext onto the 
call stack and restore them before return;. The main point is to be careful
not to use junk values after an error and let it propagate through the system.



How to add more units:
	You may want to implement a unit that is not part of the library yet. Here is
	a step by step guide on how to implement new units:

	1. In PhysicalUnitDefinitions.cpp, find the KnownUnits[] array, and add a line.
		The format should be commented in there, but just in case, you'll need
		the symbol, the long name, the dimensions/powers of 
		{mass, distance, time, temperature, charge}, the offset amount
		(probably 0, mostly used in temperature scales), and finally,
		the almighty conversion factor. Conversion factors are expressed
		in terms of kilograms, meters, seconds, Kelvin, and Coulombs.
		You remember that ice cube tray method from high school, right?
		What are they teaching you kids these days?

	2. At the end of PhysicalQuantity.cpp you will find the literal definitions.
		Add a line: DefineLiteral(symbol) // do not quote symbol
		You may choose not to define a literal if it introduces a naming conflict
		or you simply don't want to pollute the namespace. However, if you
		want your changes to be pulled into the origin repo, you should define
		the literal unless its symbol conflicts with a more popular unit.

	3. In PhysicalQuantity.h, find all the DeclareLiteral() macros at the end and add:
		DeclareLiteral(symbol)  // again, do not put in quotes

	4. Rebuild the hash tables. 
		If using the Visual Studio solution, this is done automatically by a
		custom build step. "Rebuild all" should invoke this. 
		There is a separate VC project called genhashtables, and its output is to
		be fed back into the main project as hashTables.h
			($OutputPath)/genhashtables > include/PhysicalQuantity/hashTables.h
	
	5. Examine the generated file to see if the bucket size is obnoxious, i.e. lots of 
		hashing collisions.
		If so, there is a method for reducing that size:
			a) Recompile and run genhashtables with --find-seed
			b) In PhysicalUnitDefinitions.cpp (bottom), change PhysicalQuantity::defaultHashSeed 
			   to that value.
			c) Run genhashtables again with no arguments, replacing hashTables.h,
			   or just do a full rebuild of the genhashtables project in VS.
			d) If the bucket size / collision rate is still too high, increase the hash
			   table size, also in PhysicalUnitDefinitions.cpp at the bottom.
			   Repeat from a) until satisfied.
		It may also be beneficial to modify the hashing function in hash.cpp, but this will
		have to run on embedded hardware, so don't make it too complicated.
		
	6. Recompile with the new hashTables.h. The updated data is now available on the target system.

	Note: You should never #include hashTables.h in your own code - it is meant to be inlined into
		the library code. #pragma once will not solve linker errors
