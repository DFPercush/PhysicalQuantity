include config.mk
compile=$(Compiler) $(CompilerOptions)
StaticHeaders=include/PhysicalQuantity.h include/PhysicalQuantity/ppOptions.h
AllHeaders=$(StaticHeaders) include/PhysicalQuantity/hashTables.ah include/PhysicalQuantity/literals.ah

all: $(BinPath)gencode $(LibPath)PhysicalQuantity.a $(BinPath)TestConsole

#-----------------------------------------------------------------
# Binaries
gencodeObjects=$(ObjPath)gencode.o $(ObjPath)csubstr.o $(ObjPath)hash.o $(ObjPath)PhysicalQuantity-gencode.o $(ObjPath)PhysicalUnitDefinitions.o
$(BinPath)gencode: $(gencodeObjects)
	$(compile) $(LibPathFlag)$(LibPath) $(OutputFlag) $(BinPath)gencode $(gencodeObjects)

#TestConsoleObj=$(ObjPath)TestConsole.o $(LibPath)libPhysicalQuantity.so
#$(BinPath)TestConsole: $(TestConsoleObj)
#	$(compile) $(LibPathFlag)$(LibPath)  $(OutputFlag) $(BinPath)TestConsole $(ObjPath)TestConsole.o $(LibDepFlag)PhysicalQuantity
TestConsoleObj=$(ObjPath)TestConsole.o $(LibPath)PhysicalQuantity.a
$(BinPath)TestConsole: $(TestConsoleObj)
	$(Compiler) $(OutputFlag) $(BinPath)TestConsole $(ObjPath)TestConsole.o $(LibPath)PhysicalQuantity.a

#-----------------------------------------------------------------
# Shared library
libPqObj=$(ObjPath)csubstr.o $(ObjPath)hash.o $(ObjPath)PhysicalQuantity.o $(ObjPath)PhysicalUnitDefinitions.o $(ObjPath)literals.o $(ObjPath)hashParams.o
#$(LibPath)libPhysicalQuantity.so: $(libPqObj)
$(LibPath)PhysicalQuantity.a: $(libPqObj)
	ar rvs $(LibPath)PhysicalQuantity.a $(libPqObj)

#	$(compile) $(SharedFlag) $(SharedLinkFlags) $(OutputFlag) $(LibPath)libPhysicalQuantity.so $(libPqObj)


#-----------------------------------------------------------------
# Generated files
#include/PhysicalQuantity/hashTables.ah: $(BinPath)gencode
#	$(BinPath)gencode > include/PhysicalQuantity/hashTables.ah
include/PhysicalQuantity/hashTables.ah include/PhysicalQuantity/literals.ah src/literals.acpp src/hashParams.acpp: $(BinPath)gencode
	$(BinPath)gencode generate --rootpath ./ optimize --max-seed 255 --max-table-size 100 --min-bucket-size 2


#---------------------------------------------------------------
# Intermediate objects
CommonIntCompile=$(compile) $(SharedCompileFlags) $(Defines) $(IncludePathFlag)$(IncludePath) $(OutputFlag) $(ObjPath)
$(ObjPath)PhysicalQuantity-gencode.o: src/PhysicalQuantity.cpp $(StaticHeaders)
	$(CommonIntCompile)PhysicalQuantity-gencode.o src/PhysicalQuantity.cpp $(DefineFlag)PQ_GENCODE

# For some of these modules, I'm passing the gencode define because 
# they really just don't need all the fancy stuff, and gencode
# depends on them. They mainly just need to see their types declared
# from within the PQ class. It's kind of a circular dependency situation,
# but we are tricksy. Could create separate intermediates for the gencode
# binary, but it's really not necessary.
$(ObjPath)csubstr.o: src/csubstr.cpp $(StaticHeaders)
	$(CommonIntCompile)csubstr.o src/csubstr.cpp $(DefineFlag)PQ_GENCODE

$(ObjPath)hash.o: src/hash.cpp $(StaticHeaders)
	$(CommonIntCompile)hash.o src/hash.cpp $(DefineFlag)PQ_GENCODE

$(ObjPath)PhysicalUnitDefinitions.o: src/PhysicalUnitDefinitions.cpp $(StaticHeaders)
	$(CommonIntCompile)PhysicalUnitDefinitions.o src/PhysicalUnitDefinitions.cpp $(DefineFlag)PQ_GENCODE

$(ObjPath)gencode.o: src/gencode.cpp $(StaticHeaders)
	$(CommonIntCompile)gencode.o src/gencode.cpp $(DefineFlag)PQ_GENCODE

$(ObjPath)literals.o: src/literals.acpp $(StaticHeaders) bin/gencode
	$(CommonIntCompile)literals.o $(ExplicitCppFile) src/literals.acpp

# Now these two really do need everything generated and built before them.
$(ObjPath)PhysicalQuantity.o: src/PhysicalQuantity.cpp $(AllHeaders)
	$(CommonIntCompile)PhysicalQuantity.o src/PhysicalQuantity.cpp

$(ObjPath)TestConsole.o: src/TestConsole.cpp $(AllHeaders)
	$(CommonIntCompile)TestConsole.o src/TestConsole.cpp

$(ObjPath)hashParams.o: src/hashParams.acpp bin/gencode
	$(CommonIntCompile)hashParams.o $(ExplicitCppFile) src/hashParams.acpp

clean:
	rm -f $(BinPath)*
	rm -f $(LibPath)*
	rm -f $(ObjPath)*
	rm -f src/*.acpp
	rm -f include/PhysicalQuantity/*.ah
