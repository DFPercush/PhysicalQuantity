# PhysicalQuantity
### Unit conversion and arithmetic in C++


###### by Daniel Papenburg, 2019-2020 
[github.com/DFPercush](github.com/DFPercush)

Includes a test console / calculator and a highly configurable library that can function on a potato-powered microcontroller.

>You are free to use and modify this software any way you want, just don't plagiarize or claim credit for work that's not yours. Enjoy!


Does your project leader use the metric system but you love your freedom units? Does that one guy insist on using inches instead of cm? No problem! For the cost of 5 extra bytes and a few integer operations, your measurements and calculations will never be ambiguous again! Everyone can operate in their preferred units, hard code values using gallons in one place and cc's in another, and it knows, man, it knows...

PhysicalQuantity, or PQ, is an embedded-friendly C++ class library which
allows direct arithmetic operations on values including the fundamental units of
mass, distance, time, temperature, and current, as well as
any other units derived from these quantities like force and speed.

Its test console, a stand-alone program that runs on the command line, is useful in its own right as a kind of calculator for physical values and unit conversion, but if you're a coder too, it gives you an awesome class to use in your own projects any time you're dealing with real world measurements.

The PQ class keeps track of the dimensions of a value during all calculations, and 
handles multiplication and division of units, conversion factors, and dimension checking
during addition and subtraction, as well as scalar operations.  It includes a `.parse()` function for reading a string value, an
`.eval()` function for parsing whole math expressions, as well as `.sprint()` and `.toString()` for 
generating text output, and `.convert()` for extraction of raw numeric values in code**.

> This readme serves multiple purposes, including sales pitch, installation guide, and documentation. It's gotten quite long, but if you have a question, there's a good chance it's in here somewhere. Thanks for your understanding.

** Before you read out a numeric value, consider that the point of this whole thing is to keep track
   of units at low cost while you're doing math. So it's best to keep values inside of PhysicalQuantity
   objects as long as possible, and only read it when you need to output something to a person.

Sample code:

	InitLibPQ(); // mainly for #define option checking
	// Initializing with text:
	PhysicalQuantity q("12 cm^3");
	
	// Initializing with compile-time literals:
	auto start = 1.23_km;
	auto end = 3.7_mi;
	auto speed = (end - start) / 15_min;
	cout << speed.toString(); // "7.98286 m / s"
	cout << speed.toString("mph"); // "17.8571 mph"

Sample test console output:

	C:\>pq -i -s
	expression [ , preferred output units ] | 'sprint' | 'convert' | 'quit' | 'exit'
	sprint> 3.95 A * 19 V
	75.05 W
	sprint> 25 mi / 30 min
	50 mph
	sprint> 60 mph / 3.7 seconds, gees
	0.7392225986719435 g0
	sprint> 60 mph / 3.7 seconds, m
	7.24929729726621463 m / s^2
	sprint> (120 ft^3/min) * 10 seconds, beerbarrels
	4.82614017339907608 beerbarrel


Whitespace is important!

	eval() examples:
		Bad: "2km"
		Good: "2 km"
		Good: "2 kg*m/s"   ==   "2 kg m/s"
		Bad: 1 km-2 m
		Bad: 1 km -2 m
		Good: 1 km - 2 m
		OK: 1 km - -2 m
		Note: "3 m^2"   !=   ( "3 m ^ 2" == "(3 m) ^ 2" == "9 m^2")
		Note: "5 * m^2" == "5 m^2"



===================================================================================================

### System requirements:

* C++14 recommended, but there are some #defines to make it compatible with older/simpler compilers.  
  Tested on the following systems:  
  * Windows / Microsoft Visual Studio 2019
  * Linux Mint / g++ 7.4.0
  * Arduino IDE 1.8.10 / avr-g++ 7.3.0   
    (slight tweak recommended to use C++14, see 'installation' section)

* For Arduino/avr (Uno board):
  * With `NO_TEXT`: 444 bytes flash ROM, 9 bytes SRAM.
  * With text parsing/printing and a full list of every unit under the sun, about 23 KB flash memory, 109 bytes SRAM. This grows as more units are added, but you can also remove the ones you don't need.

* For PC/Windows/Linux: (test console binary)
  * Between 70 and 200 KB memory in "Debug" configuration
  * Up to 700 KB with speed > size optimizations.

  * `log10()` in standard library  
    (Not required if using `NO_TEXT`)

  * `eval()` is recursive, stack usage depends on input.


* Optional:
  * std::string
  * new/delete dynamic memory allocation
  * try/throw/catch exception handling


There is a header file, `ppOptions.h`, which contains some conditional compilation options for limited systems.


### Installation/setup:

#### Using the test console as a personal calculator and unit converter:

* Windows / Visual Studio  
   Open the solution in `msvc/` and hit `F7` or menu  
 Build > Rebuild Solution.  
  The program is at `bin/pq.exe`

* Linux  
  Open a shell where you cloned the code and run `make`.
	    `config.mk` has several options you may be interested in, but most systems will build just fine out of the box.  
`bin/pq` is the binary.  It also contains a `gencode`
		binary used during the build process to generate the .ah and .acpp files.  
		`lib/` will contain the static library, or object archive, with the API.
		If you want to produce a `.so` shared library instead, you can change the compiler flags in
		`config.mk` - although the makefile is currently set up with a build rule 
		to use `ar` to produce an `.a` file, so that will need to be updated as well,
		using g++ instead, to make a .so.


### Using the library in other projects:

> There is a shortcut for Arduino development: run `make arduino` in linux or a cygwin shell.
Some code must be self-generated before you copy it as an arduino library. See below.

1. Follow the instructions in the previous section to build the library.

In your source files you will need:

	#include <PhysicalQuantity.h>

##### Linux/cygwin/mingw using shell and makefile:  
   Pass `-I(/path/to/PhysicalQuantity/include)`
to all your compile rules, and for the final link, `-L(path/to/PhysicalQuantity/lib)` and put `PhysicalQuantity.a` as one of the inputs.

##### Visual Studio:  
My personal method is to use property sheets to import all the settings I need
from a particular library, rather than having to arduously set up my project settings
every time I want to do a quick test.   
Open `msvc/RootDevDir.props` - this should contain the path where you put most new projects.
By default it contains a relative path from within this project's directory structure,
to make out-of-the-box builds easier. But if you always put your projects in a certain
place like I do, you can change RootDevDir to that absolute path.
Using a text editor to modify `RootDevDir.props` might be easier than trying to
navigate visual studio's property pages, but it will be in the user macros section.  
`msvc/PhysicalQuantity-IncludeLib.props` contains the settings for the include and 
library directories, relative to `$(RootDevDir)`. So, in your new project, open the 
property manager and add your modified `RootDevDir.props`, then 
PhysicalQuantity-IncludeLib.props, in that order, and your paths will be all set.
Now you can #include <PhysicalQuantity.h> instead of
#include "../../only/works/on/your/machine/PhysicalQuantity.h"


##### Cross compiling for embedded development

You will need to use the PC tool chain, outlined above in the test console section, to build and run 
`gencode` before you try to cross compile for the target system. This does several things like defining literals and generating hash tables to make text parsing faster. It also makes it a lot easier to add new units.

* Arduino IDE:
  * You will need a command line / shell environment with the `make` tool. If you're on a windows machine, install cygwin or mingw and you'll have it.
  * In this project's directory, type `make arduino`
  * Create a folder for the library in your arduino project folder under `libraries/`
  * Copy the contents of `PhysicalQuantity/arduino/` from the  folder to a the new library folder
  * (recommended) Change the compiler options to use C++14
    * Find the folder/directory where you installed the arduino software
    * Open `arduino-1.x.x\hardware\arduino\avr\platform.txt`
    * On the line that begins with `compiler.cpp.flags`, change `-std=gnu++11` to `-std=gnu++14`. If you don't see that option, add it.
    * `#include "PhysicalQuantity.h"` and start coding!  
		  (Note the use of `"quotes"` rather than `<brackets>` )


### Keeping git clean
If you make changes to any config files for your specific system, and you think you
might be creating new commits and pull requests in git,
you will need to tell git not to track changes to certain config files. 
This can be done with:

	git update-index --skip-worktree (file)

If you need to change Visual Studio project configuration, it's a good idea to create a new
property sheet and put the settings there, so you're not committing your personal settings in the shared git repo.  
The menu is View > Property Manager. In that window, Right click your project and Add New Item > Property Sheet. Any changes you make there will override the default settings, and you can change the order in which they apply. Search the Microsoft docs for more on Visual Studio property sheets if this is new to you.




### Conditional compilation options / #defines

`ppOptions.h` contains many useful options for controlling how many resources are required by this library. By default, everything is enabled, but you can trim it way, way down. The header file itself contains an explanation of each one, so have a look there. For any of these options, you can either uncomment the related line
in `ppOptions.h`, or you can `#define` the option in your source file before `#include`ing 
the header, or you can pass the `#define` as an argument to the compiler. All this 
flexibility does makes it necessary, however, to check that various modules are 
compiled with the same configuration as their dependencies. There is an easy macro 
for this purpose called `PQHeaderOptionsMatch`.  Here's an example:

	#include <PhysicalQuantity.h>
	int main() {
		if (!PQHeaderOptionsMatch) {
			printf("Error: library code was compiled with different header options.\n");
			return 1;
		}
		...
	}


### Performance notes:

Internal math operations are very fast (as long as you have floating point
hardware), but when you want to parse a value in, or print one out, some lookups
are necessary to convert between units. A lot of the preprocessor options deal
with how this particular aspect is handled. You can have a hash table in ROM to
make unit lookups faster, or use a slower linear search for minimum memory 
overhead. There is also a cache mechanism for storing parsed unit strings.
See UnitList.

Performing arithmetic with these objects, once parsed, basically
involves one floating point operation and 5 integer additions/comparisons.
In the case of `eval()`'ing an exponentiation, that's 5 integer multiplies and a `pow()`.
But `eval()` has overhead anyway, particularly if you're fond of parentheses.
Note that there's no built in exponent operator, but you can use `sprint()` + `eval()`
to accomplish this. A word of caution, though: values with units can only be raised
to integer exponents. 1 meter ^ 2.3  will throw an error. Moving on...

As far as the memory footprint, it varies widely depending on your system,
and which options you enable. The library itself takes hardly any RAM, and values
are most likely 12 or 16 bytes each.  
`gencode info` will tell you the exact size
with padding taken into account. Not so accurate for cross compilation though.
	The hash tables and unit definitions can be placed in ROM if available.
This mode is controlled by the `ROM_READ_BYTE` and `DECLARE_CONST_ARRAY` macros. See `ppOptions.h`
The header will recognize the arduino environment automatically via
`ARDUINO` being #defined, so you shouldn't have to to anything else.
	You can also save a lot of space by #define'ing `NO_TEXT`. Most of the code
size is dealing with input, output, and conversion of text.


### Q&A

Q: Can a PhysicalQuantity be binary copied with memcpy() or DMA?  
A: Yes.

Q: Can it be sent over a network in binary?  
A: I wrote a long answer for this, then I wrote readNetworkBinary() and
   writeNetworkBinary(). Byte order matters, basically.
   Do make sure the other end is using the same version of this library,
   with the same #define options, to load the value,
   and not just reading a double. That is the whole point after all.
   It might be safer to just `toString()`/`sprint()` and `parse()` it on the other end.

Q: Does the structure store references or pointers to any external buffers?  
A: Not the main data type, no.  
   But the UnitList class which is used for formatting text output 
   requires some buffer storage. In static mode (`NO_NEW`), you will need
   to supply this buffer. You can choose to explicitly use `UnitList_static`
   any time, but `UnitList_dynamic` is only available `#ifndef NO_NEW`.
   `UnitList` is a `typedef` that conditionally aliases to either the static
   or dynamic version.

Q: Can I use the text from `toString()` to construct the value again?  
A: Yes, that is one way to serialize the data. Beware of floating point
   precision issues, though.

Q: If I `eval()` to load a string in, and then `toString()`, will I get the same text out?  
A: Not necessarily. You can tell `toString()` and `sprint()` what units to use,
   but if you do not specify, it will do its best to construct a human readable
   output using the fewest composite units. This will probably be in SI/metric.
   Your desired units can be cached in a UnitList object for speed's sake,
   but conversion factors are always applied when parsing and printing, though
   that factor may be 1. That conversion may introduce slight precision errors.
   P.S. - A UnitList is not stored as a part of the value, that is only a conversion
   mechanism. Also, whitespace may be different.



#### I just want a number...
For any code in which you need to read and work with the numeric value 
directly, rather than displaying or logging it, use `convert()`.
This function is more strict than `toString()` / `sprint()`,
in that you must request a value of a specific unit, and if the dimensions of
that unit do not match the stored value, an error occurs.

For example, if you perform a calculation of energy in terms of Joules,
you can `convert("kWh")` just fine, because kWh is another unit of energy.
But trying to `convert("cm")` will produce an error.  `sprint()` or `toString()` is not strict in that sense, and treats your requested units as more of a suggestion.


##### A note about vectors:
You may decide to code a vector of `PhysicalQuantity`'s,
but the class itself can not represent a vector, nor imaginary numbers.
It only represents a single numeric value and its associated units.
You can, however, compare the dimensions of two values to see if they represent the
same kind of quantity with `a.like(b)` or `a.isa(b)`
	Any template code which performs vector math on a template type should work with
PQ objects, as most of the basic arithmetic operators are implemented, with the 
exception of modulo (%). Increment ++ and decrement -- will cause errors if the
value is not a scalar, but they are there.
Bitwise and logical operators are not supported, but boolean comparisons like > and <= are,
with the caveat that the values must have like dimensions (same kind of units) or
an error will be thrown/called.


##### A note about angles:
>Caution: Subject to change in a planned 'scalar units' update.

Since angles are dimensionless ratios (length/length), they are stored internally as a
simple scalar value. However, one can convert() this internal value to and from degrees 
or radian units. You may use the literal operators `1_deg` and `1_rad` to specify
hard coded values, as well as the nominal units of "deg" and "rad" when parsing / 
printing / convert()ing, to apply the proper conversion factor.
Currently there is no method of internally differentiating an angle from any other 
scalar value. Most quantities which are ratios of like units will exhibit this property.
(TODO: angle flag?)


##### A note about temperature:
Temperature is a special snowflake. Degrees, whether Celsius or Fahrenheit, not
only involve a conversion factor, but also a conversion offset. In other words, an
amount that must be added to the value as well as a multiplication factor. The unique
nature of these units means that certain otherwise parseable values, such as
"16 degF^2" will produce an error, never mind that the square of a temperature
doesn't really make sense. Be careful that any units with an offset (degF and degC)
only have a power of 1 or -1 when parsing. Additionally, if the value contains any
other dimensions, such as degrees C per second, the offset will not be applied and 
the internal value will represent the delta-T in Kelvin / sec, or K/m, or whatever.
1 degree C per second is 1 K / sec, not 274.
	If any science types out there think this is a bad idea, feel free to 
contact me or comment on github.




### Exception safety:
If an error occurs during an operation such as adding two dissimilar values,
the original value (left hand side) will remain unmodified. If a parsing error
occurs during the construction of an object, its value will be a scalar 0.
Errors during toString/sprint/convert will not modify the value, but you may not 
get the correct / any output.
	

### Error handling:
If your system/config allows it, the class can throw a few exceptions, all based on `std::runtime_error`. Here's what they mean:

* `UnitMismatchException` - Units do not match during add, subtract, or convert()
* `InvalidExpressionException` - There was a parsing error, like unit not found.
* `HeaderConfigException` - The #define options are different between your code and
 what the library / object was compiled with.
* `std::overflow` - unit exponent too large
* `std::logic error` - Congratulations, you found a bug.
* `std::bad_alloc` - Out of memory, or buffer too small

If `NO_THROW` is #defined, the class will declare a couple of static members which
are publicly accessible to you: `*errorHandler(...)`, which is a pointer to a function,
and `errorUserContext`, which is an opaque `void*` pointer for you to use however
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
an `enum ErrorCode`, which includes `E_SUCCESS = 0` for convenience.
How exactly you implement error handling from this point is up to you.
You may want to pass some process info as your context, and perform specific
cleanup actions related to it in case of an error. You can also use local
variables to push the state of errorHandler and errorUserContext onto the 
call stack and restore them before return;. The main point is to be careful
not to use junk values after an error and let it propagate through the system.


## How to add more units:

You may want to implement a unit that is not part of the library yet. Here is
	a step by step guide on how to implement new units:

1. In PhysicalUnitDefinitions.cpp, find the `KnownUnits[]` array, and add a line.
		The format should be commented in there, but you will need the symbol, the long name, plural of the long name, the conversion factor, and the dimensions/powers of the unit. You may see some lines periodically that look like this:

        // Ma Di Ti Te Cu


That tells you what order the dimensions are in, and it stands for  
`{` mass, distance, time, temperature, current `}`. There is an offset amount which will be zero basically always. Then finally there are some optional flags after the dimensions, which include `NOPREFIX` - telling the I/O code not to use any prefixes with that unit, as well as `NOLITERAL` which tells `gencode` not to create any user-defined literals for it.
		
The conversion factor is expressed
		in terms of kilograms, meters, seconds, Kelvin, and Amps. How many kg * m / s etc... are in your unit? If you learned the "ice cube tray" method in school, use that to convert everything to SI.

2. Rebuild All or `make` to invoke gencode.
		Or manually, it's a bit tricky, but you must first compile `gencode` and then run `./bin/gencode generate optimize`
		This will rebuild hash tables and literal definitions in code.
		The files build with this tool have extension `.ah` and `.acpp`
		the 'a' standing for automatic.
	
3. (_Optional_) Run `gencode info` or examine `hashTables.ah` to see 
		if the bucket size is obnoxious, i.e. lots of hashing collisions.
		If so, run gencode again and pass --max-table-size (a bigger number) and --max-seed (more)
		It may also be beneficial to modify the hashing function in hash.cpp, but this may
		have to run on embedded hardware, so don't make it too complicated.
		
	Note: You should never `#include hashTables.ah` in your own code, on pain of linker errors.
	      You should only ever need to `#include <PhysicalQuantity.h>` in external projects.
