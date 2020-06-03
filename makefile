include config.mk
compile=$(Compiler) $(CompilerOptions)
StaticHeaders=include/PhysicalQuantity.h include/PhysicalQuantity/ppOptions.h
AllHeaders=$(StaticHeaders) include/PhysicalQuantity/hashTables.ah include/PhysicalQuantity/literals.ah

pc: $(BinPath)gencode $(LibPath)PhysicalQuantity.a $(BinPath)TestConsole

arduino: arduino/libpq.cpp

all:  $(BinPath)gencode $(LibPath)PhysicalQuantity.a $(BinPath)TestConsole arduino/libpq.cpp


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
	cp $(BinPath)TestConsole $(BinPath)pq

#-----------------------------------------------------------------
# Shared library
libPqObj=$(ObjPath)csubstr.o $(ObjPath)hash.o $(ObjPath)PhysicalQuantity.o $(ObjPath)PhysicalUnitDefinitions.o $(ObjPath)literals.o $(ObjPath)hashParams.o $(ObjPath)romtable.o
#$(LibPath)libPhysicalQuantity.so: $(libPqObj)
$(LibPath)PhysicalQuantity.a: $(libPqObj)
	ar rvs $(LibPath)PhysicalQuantity.a $(libPqObj)

#	$(compile) $(SharedFlag) $(SharedLinkFlags) $(OutputFlag) $(LibPath)libPhysicalQuantity.so $(libPqObj)


#-----------------------------------------------------------------
# Generated files

include/PhysicalQuantity/hashTables.ah include/PhysicalQuantity/literals.ah src/literals.acpp src/hashParams.acpp src/romtable.acpp: $(BinPath)gencode
	$(BinPath)gencode generate --rootpath ./ optimize --max-seed $(HashMaxSeed) --max-table-size $(HashMaxTableSize) --min-bucket-size $(HashMinBucketSize)


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

$(ObjPath)hashParams.o: src/hashParams.acpp bin/gencode
	$(CommonIntCompile)hashParams.o $(ExplicitCppFile) src/hashParams.acpp

$(ObjPath)romtable.o: src/romtable.acpp $(AllHeaders)
	$(CommonIntCompile)romtable.o $(ExplicitCppFile) src/romtable.acpp

$(ObjPath)PhysicalQuantity.o: src/PhysicalQuantity.cpp $(AllHeaders)
	$(CommonIntCompile)PhysicalQuantity.o src/PhysicalQuantity.cpp

$(ObjPath)TestConsole.o: src/TestConsole.cpp $(AllHeaders)
	$(CommonIntCompile)TestConsole.o src/TestConsole.cpp

#----------

#sed s/#include\\\ \<\\\(Physical.\*\\\)\>/#include\\\ \\\"\\\1\\\"/ | sed s/PhysicalQuantity\\///
#sed s/#include\\\ \<\\\(Physical.\*\\\)\>/#include\\\ \\\"\\1\\\"/ | sed s/PhysicalQuantity\\\/// > arduino/PhysicalQuantity.h

MakeIncludesLocal=sed s/\#include\\\ \<\\\(Physical.\*\\\)\>/\#include\\\ \\\"\\\1\\\"/ | sed s/PhysicalQuantity\\///
arduino/libpq.cpp: $(LibPath)PhysicalQuantity.a src/hashParams.acpp src/literals.acpp include/PhysicalQuantity/hashTables.ah include/PhysicalQuantity/literals.ah src/romtable.acpp $(AllHeaders)
	cat src/csubstr.cpp | $(MakeIncludesLocal) > arduino/csubstr.cpp
	cat src/hash.cpp | $(MakeIncludesLocal) > arduino/hash.cpp
	cat src/hashParams.acpp | $(MakeIncludesLocal) > arduino/hashParams.cpp
	cat src/literals.acpp | $(MakeIncludesLocal) > arduino/literals.cpp
	cat src/PhysicalQuantity.cpp | $(MakeIncludesLocal) > arduino/PhysicalQuantity.cpp
	cat src/PhysicalUnitDefinitions.cpp | $(MakeIncludesLocal) > arduino/PhysicalUnitDefinitions.cpp
	cat src/romtable.acpp | $(MakeIncludesLocal) > arduino/romtable.cpp
	cat include/PhysicalQuantity.h | $(MakeIncludesLocal) > arduino/PhysicalQuantity.h	
	cp -f include/PhysicalQuantity/literals.ah arduino/literals.ah
	cp -f include/PhysicalQuantity/hashTables.ah arduino/hashTables.ah
	cp -f include/PhysicalQuantity/ppOptions.h arduino/
	echo "#include <avr/pgmspace.h>" >> arduino/ppOptions.h
	echo "#define NO_THROW" >> arduino/ppOptions.h
	echo "#define NO_NEW" >> arduino/ppOptions.h
	echo "#define LOW_PRECISION" >> arduino/ppOptions.h
	echo "#define NO_SPRINTF_FLOAT" >> arduino/ppOptions.h
#	cat include/PhysicalQuantity.h | sed s/#include\\\ \<\\\(Physical.\*\\\)\>/#include\\\ \\\"\\1\\\"/ | sed s/PhysicalQuantity\\\/// > arduino/PhysicalQuantity.h
#	cat src/csubstr.cpp src/hash.cpp src/hashParams.acpp src/literals.acpp src/PhysicalQuantity.cpp src/PhysicalUnitDefinitions.cpp src/romtable.acpp | sed s/#include\\\ \<\\\(Physical.\*\\\)\>/#include\\\ \\\"\\\1\\\"/ | sed s/PhysicalQuantity\\/// > arduino/libpq.cpp
#	echo "#define NO_TEXT" >> arduino/ppOptions.h


#	echo "#define DEFINE_SIZE_T" >> arduino/ppOptions.h

#cat src/csubstr.cpp src/hash.cpp src/hashParams.acpp src/literals.acpp src/PhysicalQuantity.cpp src/PhysicalUnitDefinitions.cpp | sed s/#include\\\ \<\\\(Physical.\*\\\)\>/#include\ \\\ \\\"\\\1\\\"/ > arduino/libpq.cpp
#sed s/#include\\\ \<\\\(Physical.\*\\\)\>/#include\\\ \\\"\\1\\\"/ < include/PhysicalQuantity.h > arduino/PhysicalQuantity.h

#------------
clean:
	rm -f $(BinPath)*
	rm -f $(LibPath)*
	rm -f $(ObjPath)*
	rm -f src/*.acpp
	rm -f include/PhysicalQuantity/*.ah
	rm -f arduino/*.cpp
	rm -f arduino/*.acpp
	rm -f arduino/*.h
	rm -f arduino/*.ah
	
