PhysicalQuantity
Original version by Daniel Papenburg, 2019
github.com/DFPercush
free/open source, just don't plagiarize


	PhysicalQuantity is an embedded-friendly C++ class library which
allows direct arithmetic operations on values including the basic units of
distance, mass, time, temperature, and current, as well as
any other units derived from these quantities. This allows different teams 
working on a project to operate in their preferred units without things going boom.
	It keeps track of the dimensions of a value during all calculations, and 
handles multiplication and division of units, conversion factors, and dimension checking
during addition and subtraction, as well as scalar operations.  It includes a
.eval() function for parsing whole math expressions* as well as .sprint() for 
generating text output, and .convert() for extraction of numeric values in code**.
	Test console included. 

Sample code:
	PhysicalQuantity q("12 cm^3");
	auto start = 1.23_km;
	auto end = 3.7_mi;
	auto speed = (end - start) / 15_min;
	cout << speed.toString(); // "7.98286 m / s"
	cout << speed.toString("mph"); // "17.8571 mph"

* whitespace between values is important!
	eval() examples:
		Bad: 2km
		Good: 2 km
		Bad: 1+2
		Good: 1 + 2
		Bad: (1 m+2 km)/s
		Good: (1 m + 2 ft) / s
			Note: You can add different units as long as they are like quantities.
		"3 m^2"   !=   ("3 m ^ 2" == "(3 m) ^ 2" == "9 m^2")
		"5 * m^2" == "5 m^2"

** Before you read out a numeric value, consider that the point of this whole thing is to keep track
   of units at low cost while you're doing math. So it's best to keep values inside of PhysicalQuantity
   objects as long as possible, and only read it when you need to output something to a person.

===================================================================================================
System requirements:
	- A C++ compiler. Preferably C++14, but there are some #defines to make it compatible
	   with older/simpler compilers.
	   Tested on the following systems:
	      - Windows / Microsoft Visual Studio 2017
		  - Linux Mint / g++ 7.4.0
		  - Arduino IDE 1.8.10 / avr-g++ 7.3.0 
			(slight tweak recommended to use C++14, see installation)

	- For Arduino/avr:
		With NO_TEXT: 444 bytes flash memory, 9 bytes SRAM.
		With text parsing/printing, 14.1 KB flash memory, 109 bytes SRAM.

	- For PC/Windows/Linux: ('testconsole' binary)
		Between 70 and 200 KB memory in "Debug" configuration
		Up to 700 KB with speed > size optimizations.

	- log10() in standard library
		(Not required if using NO_TEXT)

	- eval() is recursive, stack usage depends on input.


Optional, but not required:
	- std::string
	- new/delete dynamic memory allocation
	- try/catch
	- Support for sprintf("%g",...) if you want .toString() and .sprint()
		(Arduino does not support this)


ppOptions.h contains some conditional compilation options for limited systems.


===================================================================================================
Installation/setup:
	If all you want to do is build the library and use the test console:

	In Windows / Visual Studio, open the solution in msvc/ and hit Rebuild Solution.
		The project uses the 141 toolchain from VS 2017, mainly because of a stupid glitch
		in 2019pre which fights me every time I try to type a semicolon. (Keyword try. It's infuriating.)
		If you want to upgrade, tell git to ignore the vcxproj files or revert them before commit.
		This can be done with git update-index --skip-worktree <files>
		* I may forget to change this part of the README when I finally do upgrade myself, but just use common sense.

	In Linux, open a shell where you cloned the code and run 'make'.
		bin/ will contain the 'testconsole' binary, as well as the 'gencode'
		binary used to generate the .ah and .acpp files.
		lib/ will contain the static library, or object archive, with the API.
		If you want to produce a .so instead, you can change the compiler flags in
		config.mk - although the makefile is currently set up with a build rule 
		to use 'ar' to produce an .a file, so that will need to be updated as well,
		using g++ instead, to make a .so.


Including in other projects:

	#include <PhysicalQuantity.h>

	If using a shell and makefile, you just need to pass -I(/path/to/include)
to all your compile rules, and for the final link, -L(path/to/lib)
and put PhysicalQuantity.a as one of the inputs.

	For Visual Studio builds:
	My personal method is to use property sheets to import all the settings I need
from a particular library, rather than having to arduously set up my project settings
every time I want to do a quick test. 
	msvc/RootDevDir.props would contain the path where you put most new projects.
By default it contains a relative path from within this project's directory structure,
to make out-of-the-box builds easier. But if you always put your projects in a certain
place like I do, you can change RootDevDir to that absolute path.
Using a text editor to modify RootDevDir.props might be easier than trying to
navigate visual studio's property pages, but it will be in the user macros section.
	msvc/PhysicalQuantity-IncludeLib.props contains the settings for the include and 
library directories, relative to $(RootDevDir). So, in your new project, open the 
property manager and add your modified RootDevDir.props, then 
PhysicalQuantity-IncludeLib.props, in that order, and your paths will be all set.
Now you can #include <PhysicalQuantity.h> instead of
#include "../../only/works/on/your/machine/PhysicalQuantity.h"


	In the case of embedded development or cross compiling,
you will need to use the PC tool chain to build and run 
'gencode' before you try to cross compile for the target system, because the
unit lookup hash tables are not included in the repo, for space reasons.
It also auto generates code for literals like 1_km. If you plan on disabling these
features, you may not need to do this, in which case, proceed onward...

	Arduino IDE:
		- You will need a command line / shell environment with 'make'. 
			(You can still develop with the IDE, but you will need to use 'make' to build this library,
			because it self-generates certain code during the build process.)
			If you don't know what this is, download Cygwin.
		- In this project's directory, type 'make arduino'
		- Create a folder for the library in your arduino project under libraries/
		- Copy the contents of arduino/ to a the new library folder
		- (recommended) Change the compiler options to use C++14
			- Find the folder/directory where you installed the arduino software
			- Open arduino-1.x.x\hardware\arduino\avr\platform.txt
			- On the line that begins with 'compiler.cpp.flags', change or add
			  -std=gnu++11  ===>  -std=gnu++14
		- #include "PhysicalQuantity.h" and start coding!
		  (Note the use of quotes rather than brackets < > )

	If you make changes to any config files for your specific system, and you think you
might be creating new commits and pull requests in git,
you will need to tell git not to track changes to certain config files. 
This can be done with:

	git update-index --skip-worktree (file)

	If you need to change VS project configuration, it's a good idea to create a new
property sheet and put the settings there, so you're not committing
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


===================================================================================================
Performance:

	Internal math operations are very fast (as long as you have floating point
hardware), but when you want to parse a value in, or print one out, some lookups
are necessary to convert between units. A lot of the preprocessor options deal
with how this particular aspect is handled. You can have a hash table in ROM to
make unit lookups faster, or use a slower linear search for minimum memory 
overhead. There is also a cache mechanism for storing parsed unit strings.
See UnitList.
	Performing arithmetic with these objects, once parsed, basically
involves one floating point operation and 5 integer additions/comparisons.
In the case of eval()'ing an exponentiation, that's 5 integer multiplies and a pow().
But eval() has overhead anyway, particularly if you're fond of parentheses.
Note that there's no built in exponent operator, but you can use sprint() + eval()
to accomplish this. A word of caution, though: values with units can only be raised
to integer exponents. 1 meter ^ 2.3  will throw an error. Moving on...
	As far as the memory footprint, it varies widely depending on your system,
and which options you enable. The library itself takes hardly any RAM, and values
are most likely 12 or 16 bytes each. 'gencode info' will tell you the exact size
with padding taken into account. 
	The hash tables and unit definition table can be placed in ROM if available,
and typically require 2-3 KB. During my testing, the compiled binary sizes
were as follows:
	Windows x64 / Visual Studio 2017 Debug - 'testconsole.exe': 190 KB
	Linux Mint x64 / g++ 7.4.0 - 'testconsole' binary: 70 KB (no debug flags)
	Arduino IDE with NO_TEXT, 'Uno' board config: 600 bytes (!!)

	While I do my best to reduce memory footprint in terms of data, and shove as 
much as possible into ROM/.rodata, the code itself takes the most space.
If you only have 32K of storage on an embedded device, this might be a bit heavy 
for it. At the time of this writing, my test console binary (all preprocessor options 
enabled) is about ~190 KB on Windows, and ~70 KB on Linux. That might come down
with a minimal build option omitting certain text functions, WIP.

Q: Can a PhysicalQuantity be binary copied with memcpy() or DMA?
A: Yes.

Q: Can it be sent over a network in binary?
A: I wrote a long answer for this, then I wrote readNetworkBinary() and
   writeNetworkBinary(). Byte order matters, basically.
   Do make sure the other end is using this library to load the value,
   and not just reading a double. That is the whole point after all.
   It might be safer to just toString()/sprint() and parse it on the other end.

Q: Does the structure store references or pointers to any external buffers?
A: Not the main data type, no. CSubStrings don't either.
   But the UnitList class which is used for formatting text output 
   requires some buffer storage. In static mode (NO_NEW), you will need
   to supply this buffer. You can choose to explicitly use UnitList_static
   any time, but UnitList_dynamic is only available #ifndef NO_NEW.
   UnitList is a typedef that conditionally aliases to either the static
   or dynamic version.

Q: Can I use the text from toString() to construct the value again?
A: Yes, that is one way to serialize the data. Beware of floating point
   precision issues, though.

Q: If I toString() a value after I parse it, will I get the same
   number as text back out?
A: Not necessarily. You must tell toString() and sprint() what units to use
   every time you call it. If you do not specify, it will use fundamental SI
   units, which may look ugly compared to the derived unit you expect.
   This unit spec can be cached in a UnitList object for speed's sake,
   but conversions always happen on the way in and out.
   A UnitList is not stored as a part of the value, that is only a conversion
   mechanism. Also, whitespace may be different.
	If your original text uses the fundamental SI units of kg, m, s, K, and A,
   with one space in between everything, then it's quite possible to get the
   exact same string output back. Multiplying by 1.0 does not usually introduce
   precision errors.

===================================================================================================
Memory usage:
	tl;dr ./bin/gencode info
	  ^ May not be accurate if cross compiling, but will give you a ballpark estimate.
	testconsole compiles to between 70-180 KB on a x64 PC, depending on OS and compiler,
with all options on.
	Aside from the size of the code itself, there are a few read only tables which take
approx. 2-3 KB of storage. That will grow if more units are added. Otherwise,
actual data values are very light. The precision of values is governed by
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
	STL containers are strictly avoided, with the exception of std::string, which
you can disable. Dynamic memory allocation can be turned off with NO_NEW. 
The main place memory buffers are required is the UnitList class. It can operate
in dynamic or static mode.
	UnitList is how you specify the units of text output. It is basically a cache 
of unit lookups which you can reuse in subsequent toString() calls, rather than 
having to parse the units each time.
	In the static case, you need to give UnitList a storage buffer of size
sizeof(PhysicalQuantity::UnitListBase::UnitPref) * [however many units you tell it]
when constructed. Note that the size parameter is specified in bytes, not elements.
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
		{mass, distance, time, temperature, current}, the offset amount
		(probably 0, mostly used in temperature scales), and finally,
		the almighty conversion factor. Conversion factors are expressed
		in terms of kilograms, meters, seconds, Kelvin, and Coulombs.
		You remember that ice cube tray method from high school, right?
		What are they teaching you kids these days?

	2. Rebuild / make to invoke gencode.
		Or manually: ./bin/gencode generate
		This will rebuild hash tables and literal definitions in code.
		The files build with this tool have extension .ah and .acpp
		the 'a' standing for automatic.
	
	5. Examine hashTables.ah or ./bin/gencode ingo to see if the bucket size is obnoxious,
	    i.e. lots of hashing collisions.
		If so, there is a method for reducing that size:
			a) Recompile and run gencode find-seed
			b) In hash.cpp, change PhysicalQuantity::defaultHashSeed to that value.
			c) Rebuild or make to run 'gencode generate' again. gencode needs to
			   be recompiled with the new value before you run it.
			d) If the bucket size / collision rate is still too high, increase the hash
			   table size, also in hash.cpp
			   Repeat from a) until satisfied.
		It may also be beneficial to modify the hashing function in hash.cpp, but this may
		have to run on embedded hardware, so don't make it too complicated.
		
	Note: You should never #include hashTables.ah in your own code, on pain of linker errors.
	      You should only ever need to #include <PhysicalQuantity.h> in external projects.
