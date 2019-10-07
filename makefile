include config.mk
compile=$(Compiler) $(CompilerOptions)
StaticHeaders=include/PhysicalQuantity.h include/PhysicalQuantity/ppOptions.h
AllHeaders=$(StaticHeaders) include/PhysicalQuantity/hashTables.h

all: $(BinPath)genhashtables $(LibPath)PhysicalQuantity.a $(BinPath)TestConsole
	echo Up to date.

#-----------------------------------------------------------------
# Binaries
genhashtablesObjects=$(ObjPath)genhashtables.o $(ObjPath)csubstr.o $(ObjPath)hash.o $(ObjPath)PhysicalQuantity-gen.o $(ObjPath)PhysicalUnitDefinitions.o
$(BinPath)genhashtables: $(genhashtablesObjects)
	$(compile) $(LibPathFlag)$(LibPath) $(OutputFlag) $(BinPath)genhashtables $(genhashtablesObjects)

#TestConsoleObj=$(ObjPath)TestConsole.o $(LibPath)libPhysicalQuantity.so
#$(BinPath)TestConsole: $(TestConsoleObj)
#	$(compile) $(LibPathFlag)$(LibPath)  $(OutputFlag) $(BinPath)TestConsole $(ObjPath)TestConsole.o $(LibDepFlag)PhysicalQuantity
TestConsoleObj=$(ObjPath)TestConsole.o $(LibPath)PhysicalQuantity.a
$(BinPath)TestConsole: $(TestConsoleObj)
	$(Compiler) $(OutputFlag) $(BinPath)TestConsole $(ObjPath)TestConsole.o $(LibPath)PhysicalQuantity.a

#-----------------------------------------------------------------
# Shared libraries
pqObj=$(ObjPath)csubstr.o $(ObjPath)hash.o $(ObjPath)PhysicalQuantity.o $(ObjPath)PhysicalUnitDefinitions.o
#$(LibPath)libPhysicalQuantity.so: $(pqObj)
$(LibPath)PhysicalQuantity.a: $(pqObj)
	ar rvs $(LibPath)PhysicalQuantity.a $(pqObj)

#	$(compile) $(SharedFlag) $(SharedLinkFlags) $(OutputFlag) $(LibPath)libPhysicalQuantity.so $(pqObj)


#-----------------------------------------------------------------
# Generated files
#include/PhysicalQuantity/hashTables.h: $(BinPath)genhashtables
#	$(BinPath)genhashtables > include/PhysicalQuantity/hashTables.h
include/PhysicalQuantity/hashTables.h include/PhysicalQuantity/literals.h src/literals.cpp: $(BinPath)genhashtables
	$(BinPath)genhashtables --rootpath ./


#---------------------------------------------------------------
# Intermediate objects
CommonIntCompile=$(compile) $(SharedCompileFlags) $(Defines) $(IncludePathFlag)$(IncludePath) $(OutputFlag) $(ObjPath)
$(ObjPath)PhysicalQuantity-gen.o: src/PhysicalQuantity.cpp $(StaticHeaders)
	$(CommonIntCompile)PhysicalQuantity-gen.o src/PhysicalQuantity.cpp $(DefineFlag)PQ_BUILD_TOOL

$(ObjPath)PhysicalQuantity.o: src/PhysicalQuantity.cpp $(AllHeaders)
	$(CommonIntCompile)PhysicalQuantity.o src/PhysicalQuantity.cpp

$(ObjPath)csubstr.o: src/csubstr.cpp $(StaticHeaders)
	$(CommonIntCompile)csubstr.o src/csubstr.cpp

$(ObjPath)hash.o: src/hash.cpp $(StaticHeaders)
	$(CommonIntCompile)hash.o src/hash.cpp

$(ObjPath)PhysicalUnitDefinitions.o: src/PhysicalUnitDefinitions.cpp $(StaticHeaders)
	$(CommonIntCompile)PhysicalUnitDefinitions.o src/PhysicalUnitDefinitions.cpp

$(ObjPath)TestConsole.o: src/TestConsole.cpp $(AllHeaders)
	$(CommonIntCompile)TestConsole.o src/TestConsole.cpp

$(ObjPath)genhashtables.o: src/genhashtables.cpp $(StaticHeaders)
	$(CommonIntCompile)genhashtables.o src/genhashtables.cpp

clean:
	rm -f $(BinPath)*
	rm -f $(LibPath)*
	rm -f $(ObjPath)*

