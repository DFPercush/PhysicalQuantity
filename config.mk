# Here you can pass conditional compilation options as seen 
#in the README, if you don't want to modify ppOptions.h.

Defines=


# In case you use a different compiler
Compiler=g++
CompilerOptions=-Wall -Werror
OutputFlag=-o
SharedFlag=-shared
SharedCompileFlags=-fpic -c
SharedLinkFlags=
LibPathFlag=-L
IncludePathFlag=-I
LibDepFlag=-l
DefineFlag=-D
ExplicitCppFile=-x c++

# I don't recommend changing these paths
# You will also have to modify the buildtool source
IncludePath=include/
LibPath=lib/
ObjPath=intermediate/
BinPath=bin/
