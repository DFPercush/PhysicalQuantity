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
during addition and subtraction, as well as scalar operations. 


System requirements:
	All you really need is a rudimentary C++ compiler that can handle classes with 
overloaded operators. There are several preprocessor options for compilers 
and target systems which lack certain features like 'new' and 'printf'.
Some of the nice features you can use if you have them are:

	- (optional) literal operators (C++11), with preprocessor option 
		e.g. 12_km
		#define NO_LITERALS to disable

	- (optional) 'new' dynamic memory allocation can be utilized for convenience
		during text output / toString(), but is not strictly required
		#define NO_NEW to disable

	- (optional) try/throw/catch exception handling
		See "Error handling" below
		#define NO_THROW to use error callback instead

	- (optional) approx. 1KB of ROM space for hash tables.
	    This speeds up parsing and output.
		#define NO_HASHING to disable.

	- Does not require templates or STL support. 
	
	- std::string toString() is optional. Can write text output to a static buffer 
	   via sprint(buf, len);

	- Comes with a CSubString class (alias csubstr) which has a lot of the
	  functionality of std::strings without the memory overhead. It stores
	  a pointer and offsets to a single immutable source string, and can be
	  useful for read only operations like parsing, chopping, slicing and dicing.

	For any of these #define options, you can either uncomment the related line
in the header, you can #define the option in your source file before #including 
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



Performance:
	Internal math operations are very fast (as long as you have floating point
hardware), but when you want to parse a value in, or print one out, some lookups
are necessary to convert between units. A lot of the preprocessor options deal
with how this particular aspect is handled. You can have a hash table in ROM to
make unit lookups faster, or use a slower linear search for minimum memory 
overhead.

Q: Can a PhysicalQuantity be binary-copied with memcpy() or DMA?
A: Yes, but beware of invalid values resulting from parsing errors
    during construction. 

Q: Can it be sent over a network in binary?
A: Sort of. I would recommend using the htond() or htonf() function from 
   socket.h / winsock.h to ensure that you don't have problems with endianness,
   but if you know for sure that both machines use the same byte order, you could
   just dump (&x, sizeof(x)) to the send buffer, if you don't mind the extra overhead
   of padding bytes taking up bandwidth. To do things properly for the
   general case, you will need to declare a friend function in the header to have 
   access to the raw value, then send htond(x.value), and raw copy x.dim.

Q: Does the structure store references or pointers to any external buffers?
A: No, but the PreferredUnits class which is used for formatting text output 
   requires some buffer storage. The main PhysicalQuantity class that stores
   and manipulates physical values is self-contained.


Memory usage:
	A low memory footprint is paramount to this design, with options to reduce
memory usage even further. The precision of values is governed by the typedef 
'num' in the main header file. By default, this is 'double'.
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
more involved than a simple preprocessor option. I didn't see the need, but
that's how I would go about it if I had to.
	STL containers and dynamic memory allocation are strictly avoided if you
#define NO_NEW. The main place memory buffers are required is the PreferredUnits
class. It can operate in dynamic or static mode. PreferredUnits 
is how you specify the units of text output. It is basically a cache of unit 
lookups which you can reuse in subsequent toString() calls, rather than having
to parse the units each time.
	In the static case, you need to give PreferredUnits 
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
Errors during toString/sprint will not modify the value, but you may not get
the correct / any output.
	

Error handling:
	By default, unless NO_THROW is #defined, the code can throw a couple of errors: 
UnitMismatchException, and InvalidExpressionException. Both of these types inherit
from std::exception, so if you don't want any standard library stuff in your code,
you may want to use the next option.
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
		PQ makeAnError = 1.2_km + 3.4_kg;
		if (g_pqErr) { g_pqErr = 0; return -1; }
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


Adding more units:
	You may want to implement a unit that is not part of the library yet. Here is
	a step by step guide on how to implement new units:
	1. In PhysicalQuantity.cpp, find the KnownUnits[] array, and add a line.
		The format should be commented in there, but just in case, you'll need
		the symbol, the long name, the dimensions/powers of 
		{mass, distance, time, temperature, charge}, the offset amount
		(probably 0, mostly used in temperature scales), and finally,
		the almighty conversion factor. Conversion factors are expressed
		in terms of kilograms, meters, seconds, Kelvin, and Coulombs.
		You remember that ice cube tray method from high school, right?
		What are they teaching you kids these days?

	2. Immediately below KnownUnits[] you will find the literal definitions.
		Add a line: DefineLiteral(symbol) // do not quote symbol
		You may choose not to define a literal if it introduces a naming conflict
		or you simply don't want to pollute the namespace. However, if you
		want your changes to be pulled into the origin repo, you should define
		the literal unless its symbol conflicts with a more popular unit.

	3. In PhysicalQuantity.h, find all the DeclareLiteral() macros and add:
		DeclareLiteral(symbol)  // again, do not put in quotes

	4. Rebuild the hash tables. Compile the VC project or 'make' on a PC 
		and invoke with the command line "hash". e.g. 
			./PhysicalQuantity.exe hash > include/PhysicalQuantity/hashTables.h
	
	5. Examine the generated file to see if the bucket size is obnoxious, i.e. lots of 
		hashing collisions.
		If so, you may want to increase the hash table size for the offender in hash.h.
		It may also be beneficial to modify the hashing function in hash.cpp, but this will
		have to run on your embedded hardware, so don't make it too complicated.
		If your directory structure is different, move this file into the proper include path.
		
	6. Recompile with the new hashTables.h. The updated data is now available on the target system.

	Note: You should never #include hashTables.h in your own code - it is meant to be inlined into
		the library code. #pragma once will not solve linker errors
