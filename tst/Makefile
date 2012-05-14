LLVM_LIBDIR := $(shell llvm-config --libdir)
LLVM_OBJ_ROOT := $(shell llvm-config --obj-root)

SUT_OUTPUT := fanout_ut

SUT_ROOT := ..
SUT_SRC_DIRS := .

SUT_CPPFLAGS := -I../../../include -I../../../../../include\
                -I../../../../../build/include -I../../../../../build/tools/clang/include\
                -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS

SUT_TST_CXXFLAGS := -I.. -Wextra

SUT_LDFLAGS := -L$(LLVM_LIBDIR) -lLLVMSupport -ldl

include GTestWorker.mk

product:
	$(SILENCE)$(MAKE) -C $(LLVM_OBJ_ROOT)/tools/clang/tools/function-fanout
