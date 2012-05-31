ifndef GMOCK_ROOT
  $(error Please set GMOCK_ROOT to GoogleMock root dir)
endif
# I assume gtest is there, otherwises even gmock can't be built
GTEST_ROOT := $(GMOCK_ROOT)/../gtest

ifndef SUT_ROOT
  SUT_ROOT := .
endif

ifndef SUT_TST_DIRS
  SUT_TST_DIRS := tst
endif

ifndef SUT_SRC_DIRS
  SUT_SRC_DIRS := src
endif

ifndef SUT_OUTPUT
  SUT_OUTPUT := sut_output
endif

ifndef SUT_OBJS_DIR
  SUT_OBJS_DIR := _objs
endif
WK_OBJS_DIR := $(SUT_ROOT)/$(SUT_OBJS_DIR)
WK_OUTPUT := $(WK_OBJS_DIR)/$(SUT_OUTPUT)

### Macros ###
get_src = $(wildcard $1/*.cpp) $(wildcard $1/*.cc)
src_to_obj = $(patsubst $(SUT_ROOT)/%,$(WK_OBJS_DIR)/%, $(subst .cc,.obj, $(subst .cpp,.obj,$1)))

### Definations ###
WK_ALL_TST_DIRS := $(foreach x,$(SUT_TST_DIRS),$(SUT_ROOT)/$x)
WK_ALL_TST_FILES := $(foreach x,$(WK_ALL_TST_DIRS),$(call get_src,$x))
WK_ALL_TST_OBJS := $(foreach x,$(WK_ALL_TST_FILES),$(call src_to_obj,$x))

WK_SUT_SRC_LIB := $(WK_OBJS_DIR)/libSUT.a
WK_ALL_SRC_DIRS := $(foreach x,$(SUT_SRC_DIRS),$(SUT_ROOT)/$x)
WK_ALL_SRC_FILES := $(foreach x,$(WK_ALL_SRC_DIRS),$(call get_src,$x))
WK_ALL_SRC_OBJS := $(foreach x,$(WK_ALL_SRC_FILES),$(call src_to_obj,$x))

GMOCK_MAIN_LIB := $(GMOCK_ROOT)/make/gmock_main.a

### Options ###
LDFLAGS := -lpthread $(SUT_LDFLAGS)
CPPFLAGS := -g -Wall $(SUT_CPPFLAGS)
WK_SRC_CXXFLAGS := $(SUT_SRC_CXXFLAGS)
WK_TST_CXXFLAGS := -I$(GTEST_ROOT)/include -I$(GMOCK_ROOT)/include $(SUT_TST_CXXFLAGS)
 
### Targets ###
.PHONY: all compile clean veryclean
all: compile
	./$(WK_OUTPUT)

compile: $(WK_OUTPUT)

clean:
	-$(SILENCE)rm -rf $(WK_OUTPUT) $(SUT_CLEAN_TARGETS)
	-$(SILENCE)rm -rf $(WK_OBJS_DIR)

veryclean: clean
	-$(SILENCE)rm -rf $(SUT_VERYCLEAN_TARGETS)

$(WK_OUTPUT): $(WK_ALL_TST_OBJS) $(WK_SUT_SRC_LIB) $(GMOCK_MAIN_LIB)
	@echo linking [$@]
	$(SILENCE)$(CXX) -o $@ $^ $(LDFLAGS) $(TARGET_ARCH)

$(WK_ALL_SRC_OBJS): $(WK_OBJS_DIR)/%.obj: $(SUT_ROOT)/%.cpp
	@echo compiling [$<]
	$(SILENCE)mkdir -p $(@D)
	$(SILENCE)$(COMPILE.cpp) $(WK_SRC_CXXFLAGS) $(OUTPUT_OPTION) -MMD -MP -MT "$@ $(subst .obj,.d,$@)" $<

$(WK_ALL_TST_OBJS): $(WK_OBJS_DIR)/%.obj: $(SUT_ROOT)/%.cc
	@echo compiling [$<]
	$(SILENCE)mkdir -p $(@D)
	$(SILENCE)$(COMPILE.cpp) $(WK_TST_CXXFLAGS) $(OUTPUT_OPTION) -MMD -MP -MT "$@ $(subst .obj,.d,$@)" $<

$(WK_SUT_SRC_LIB): $(WK_ALL_SRC_OBJS)
	@echo archiving [$@]
	$(SILENCE)$(AR) $(ARFLAGS) $@ $^

$(GMOCK_MAIN_LIB):
	@echo building [$@]
	$(SILENCE)$(MAKE) -e -C $(GMOCK_ROOT)/make

