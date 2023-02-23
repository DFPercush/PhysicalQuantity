cmake_minimum_required(VERSION 3.24)

project(pq VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


option(BUILD_ARDUINO_SKETCH "Generate an Arduino IDE library folder." OFF)
#set(FOO true CACHE BOOL "description")
set(PQ_NUM_TYPE "double" CACHE STRING "Main numerical type")
option(LOW_PRECISION "If 1e100 is an overflow" OFF)
option(NO_TEXT "Disable text input parsing and output, saves space" OFF)
option(NO_LITERALS "Do not define literal operators like 1_kg." OFF)
option(NO_CONSTEXPR "No C++14 constexpr support, C++11 is not enough" OFF)
option(NO_TYPEDEFS "Do not typedef PQ and csubstr" OFF)
option(NO_NEW "Do not use dynamic memory allocation." OFF)
option(NO_STD_STRING "Do not use std::string." OFF)
option(NO_SPRINTF_FLOAT "Standard library does not support sprintf(\"%g\", ...)" OFF)
option(NO_INLINE "Do not use inline functions, make all functions normal calls." OFF)
set(INLINE_KEYWORD "inline" CACHE STRING "Keyword to use for inline functions")
option(NO_HASHING "Do not generate hash tables for unit string lookups. Saves memory at the cost of run time. Tables can be in ROM." OFF)
option(NO_THROW "Do not use 'throw' for errors, instead use an error callback (see errorHandler)" OFF)
option(NO_LONG_NAMES "Do not include full names and plurals for units and prefixes." OFF)
option(NO_STD_MAP "Do not use std::unordered_map, necessary for variables feature" OFF)
option(NO_STD_CHRONO "Do not #include <chrono> - std::chrono::duration can be cast and assigned." OFF)
set(MAX_SPRINT_UNITS "10" CACHE STRING "Max number of units that can appear in printed output")
set(MAX_NUM_TEXT_LENGTH "32" CACHE STRING "Max length of a printed number in bytes")
set(PQ_MAX_RATIO "32" CACHE STRING "Max root of a physical unit. See ppOptions.h.in for more." 32)
OPTION(OPTIMIZE_HASHES "Find optimal hash parameters for unit string lookups" OFF)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src)
configure_file(include/PhysicalQuantity/ppOptions.h.in include/PhysicalQuantity/ppOptions.h)


include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(gencode
	src/PhysicalQuantity.cpp
	src/PhysicalUnitDefinitions.cpp
	src/csubstr.cpp
	src/gencode.cpp
	src/hash.cpp
	include/PhysicalQuantity.h
	${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/ppOptions.h
)

set_property(TARGET gencode PROPERTY COMPILE_DEFINITIONS PQ_GENCODE NO_SKETCH)

if(${OPTIMIZE_HASHES})
	set(OPTIMIZE_ARGS "optimize --max-seed 255")
else()
	set(OPTIMIZE_ARGS "")
endif()

add_custom_command(
	OUTPUT
		${CMAKE_CURRENT_BINARY_DIR}/src/hashParams.cpp
		${CMAKE_CURRENT_BINARY_DIR}/src/literals.cpp
		${CMAKE_CURRENT_BINARY_DIR}/src/romtable.cpp
		${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/literals.h
		${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/hashTables.h

	#COMMAND ${CMAKE_CURRENT_BINARY_DIR}/gencode generate --rootpath ${CMAKE_CURRENT_BINARY_DIR} optimize --max-seed 255 info
	#COMMAND ${CMAKE_CURRENT_BINARY_DIR}/gencode generate --rootpath ${CMAKE_CURRENT_BINARY_DIR} info
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/gencode generate --rootpath ${CMAKE_CURRENT_BINARY_DIR} ${OPTIMIZE_ARGS} info
	#MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/gencode
)

add_custom_target(generated_files
	DEPENDS
		${CMAKE_CURRENT_BINARY_DIR}/src/hashParams.cpp
		${CMAKE_CURRENT_BINARY_DIR}/src/literals.cpp
		${CMAKE_CURRENT_BINARY_DIR}/src/romtable.cpp
		${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/literals.h
		${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/hashTables.h
)
add_dependencies(generated_files gencode)


set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/src/hashParams.cpp PROPERTY GENERATED 1)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/src/literals.cpp PROPERTY GENERATED 1)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/src/romtable.cpp PROPERTY GENERATED 1)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/hashTables.h PROPERTY GENERATED 1)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/literals.h PROPERTY GENERATED 1)

if (${BUILD_ARDUINO_SKETCH})
	add_custom_target(arduino_sketch ALL)
	#	COMMAND cp -f ${CMAKE_SOURCE_DIR}/src/csubstr.cpp                                                   ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/
	#	COMMAND cp -f ${CMAKE_SOURCE_DIR}/src/hash.cpp                                                      ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/
	#	COMMAND cp -f ${CMAKE_SOURCE_DIR}/src/PhysicalQuantity.cpp                                          ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/
	#	COMMAND cp -f ${CMAKE_SOURCE_DIR}/src/PhysicalUnitDefinitions.cpp                                   ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/
	#	COMMAND cp -f ${CMAKE_SOURCE_DIR}/include/PhysicalQuantity.h                                        ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/
	#	COMMAND cp -f ${CMAKE_CURRENT_BINARY_DIR}/src/hashParams.cpp                    ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/
	#	COMMAND cp -f ${CMAKE_CURRENT_BINARY_DIR}/src/literals.cpp                      ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/
	#	COMMAND cp -f ${CMAKE_CURRENT_BINARY_DIR}/src/romtable.cpp                      ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/
	#	COMMAND cp -f ${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/literals.h   ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/
	#	COMMAND cp -f ${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/hashTables.h ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/
	#	COMMAND cp -f ${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/ppOptions.h  ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/
	#)

	FILE(COPY_FILE ${CMAKE_SOURCE_DIR}/src/csubstr.cpp                               ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/ ONLY_IF_DIFFERENT)
	FILE(COPY_FILE ${CMAKE_SOURCE_DIR}/src/hash.cpp                                  ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/ ONLY_IF_DIFFERENT)
	FILE(COPY_FILE ${CMAKE_SOURCE_DIR}/src/PhysicalQuantity.cpp                      ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/ ONLY_IF_DIFFERENT)
	FILE(COPY_FILE ${CMAKE_SOURCE_DIR}/src/PhysicalUnitDefinitions.cpp               ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/ ONLY_IF_DIFFERENT)
	FILE(COPY_FILE ${CMAKE_SOURCE_DIR}/include/PhysicalQuantity.h                    ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/ ONLY_IF_DIFFERENT)
	FILE(COPY_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/hashParams.cpp                    ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/ ONLY_IF_DIFFERENT)
	FILE(COPY_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/literals.cpp                      ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/ ONLY_IF_DIFFERENT)
	FILE(COPY_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/romtable.cpp                      ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/ ONLY_IF_DIFFERENT)
	FILE(COPY_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/literals.h   ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/ ONLY_IF_DIFFERENT)
	FILE(COPY_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/hashTables.h ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/ ONLY_IF_DIFFERENT)
	FILE(COPY_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/ppOptions.h  ${CMAKE_CURRENT_BINARY_DIR}/arduino/PhysicalQuantity/ ONLY_IF_DIFFERENT)
	add_dependencies(arduino_sketch generated_files)
else() # if (${BUILD_ARDUINO_SKETCH})


	add_library(PhysicalQuantity
		src/PhysicalQuantity.cpp
		src/PhysicalUnitDefinitions.cpp
		src/csubstr.cpp
		src/hash.cpp
		${CMAKE_CURRENT_BINARY_DIR}/src/hashParams.cpp
		${CMAKE_CURRENT_BINARY_DIR}/src/literals.cpp
		${CMAKE_CURRENT_BINARY_DIR}/src/romtable.cpp
		include/PhysicalQuantity.h
		${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/hashTables.h
		${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/literals.h
		${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/ppOptions.h
	)


	add_executable(pq
		src/PhysicalQuantity.cpp
		src/PhysicalUnitDefinitions.cpp
		src/TestConsole.cpp
		src/csubstr.cpp
		src/hash.cpp
		${CMAKE_CURRENT_BINARY_DIR}/src/hashParams.cpp
		${CMAKE_CURRENT_BINARY_DIR}/src/literals.cpp
		${CMAKE_CURRENT_BINARY_DIR}/src/romtable.cpp
		include/PhysicalQuantity.h
		${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/hashTables.h
		${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/literals.h
		${CMAKE_CURRENT_BINARY_DIR}/include/PhysicalQuantity/ppOptions.h
	)


	set_property(TARGET PhysicalQuantity PROPERTY COMPILE_DEFINITIONS NO_SKETCH)
	set_property(TARGET pq PROPERTY COMPILE_DEFINITIONS NO_SKETCH)
	add_dependencies(pq PhysicalQuantity)
	add_dependencies(PhysicalQuantity generated_files)
	target_link_libraries(pq PRIVATE PhysicalQuantity readline)


	add_executable(test
		test/test.cpp
	)
	set_property(TARGET test PROPERTY COMPILE_DEFINITIONS NO_SKETCH)
	add_dependencies(test PhysicalQuantity)
	target_link_libraries(test PRIVATE PhysicalQuantity)


endif() # if (${BUILD_ARDUINO_SKETCH}) / else



