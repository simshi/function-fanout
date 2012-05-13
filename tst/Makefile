SUT_OUTPUT := ut
SUT_SRC_DIRS := 

WK_OBJS_DIR := _objs
WK_SRC_DIRS := $(foreach d,$(SUT_SRC_DIRS),$(SUT_ROOT)/$d)

.PHONY: all compile clean
all: $(SUT_OUTPUT)
	./$<

compile: $(SUT_OUTPUT)
clean:
	$(SILENCE)rm -rf $(WK_OBJS_DIR)

product:
	$(SILENCE)$(MAKE) -C ~/workspace/llvm-project/rel30/build/tools/clang/tools/function-fanout