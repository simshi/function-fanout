# This file requires the clang build system, at least for now. So to use this
# Makefile, you should execute the following commands to copy this directory
# into a clang checkout:
#
#   cp -R <this directory> <llvm root dir>/tools/clang/tools/
#   cd <llvm root dir>/build/tools/clang/tools/function-fanout
#   make

CLANG_LEVEL := ../..
LIBRARYNAME = FunctionFanout

LINK_LIBS_IN_SHARED = 0
SHARED_LIBRARY = 1

include $(CLANG_LEVEL)/Makefile

