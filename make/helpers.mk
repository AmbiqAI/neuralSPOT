
# WSLENV ?= notwsl
UNAME_R := $(shell uname -r)
ifneq ($(filter %WSL2,$(UNAME_R)),)
EXEEXT:=.exe
endif

# ifndef WSLENV
# # Windows and WSL are a mess. Add executable extension when
# # running in WSL.
# EXEEXT = .exe
# endif

source-to-object = $(addprefix $(BINDIR)/,$(subst .c,.o,$(filter %.c,$1))) \
                   $(addprefix $(BINDIR)/,$(subst .s,.o,$(filter %.s,$1))) \
                   $(addprefix $(BINDIR)/,$(subst .cc,.o,$(filter %.cc,$1))) \
                   $(addprefix $(BINDIR)/,$(subst .cpp,.o,$(filter %.cpp,$1))) \

src-to-build = $(subst src,$(BINDIR),$1)

# $(subdirectory)
subdirectory = $(patsubst %/module.mk,%,                        \
                 $(word                                         \
                   $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

# $(call make-library, library-name, source-file-list)
# Only add the library to the list of libraries to link in
# if the subdirectory matches the evb name.
# ifeq ($(PART)_$(EVB),$(lastword $(subst /, ,$(dir $1))))


# ifeq ($(PART)_$(EVB),$(lastword $(subst /, ,$(dir $1))))
# 	$(info $(PART)_$(EVB) ++ $1)
# libraries += $1
# endif
# nest_libraries += $1
define make-library
libraries += $1
$1: $(call source-to-object,$2)
	@echo " Building $(AR) $$@ to make library $$@"
	@mkdir -p $$(@D)
	$(Q) $(AR) $(ARFLAGS) $$@ $$^
endef

# Make one library for each target at targetname/libname.a
define make-libraries
$(foreach target,$(TARGETS),$(call make-library,$(2)/$(target)/$(1),$(3)))
endef

define make-override-library
override_libraries += $1
# sources   += $2
$1: $(call source-to-object,$2)
	@echo " Building $(AR) $$@ to make override library $$@"
	@mkdir -p $$(@D)
	$(Q) $(AR) $(ARFLAGS) $$@ $$^
endef

define make-example-library
example_libraries += $1
# sources   += $2
$1: $(call source-to-object,$2)
	@echo " Building $(AR) $$@ to make example library $$@"
	@mkdir -p $$(@D)
	$(Q) $(AR) $(ARFLAGS) $$@ $$^
endef

# # $(call make-axf, axf-name, source-file-list)
# define make-axf
# $1.axf: $(call source-to-object,$2) $(libraries) $(libraries) $(override_libraries)
# 	@echo " Helper Linking $(COMPILERNAME) $$@"
# 	@mkdir -p $$(@D)
# ifeq ($(TOOLCHAIN),arm)
# 	$(Q) $(LD) $(ARMLINKER_IS_NO_BUENO) $$^ $(override_libraries) $(lib_prebuilt) $(libraries) $(LFLAGS) --list=$*.map -o $$@
# else
# # $(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $$@ $$^ $(LFLAGS)
# 	$(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $$@ $(LFLAGS)
# endif
# endef

# $(call make-axf, axf-name, source-file-list)
define make-axf

$1.axf: $(call source-to-object,$2)  $(ARMLINKER_IS_NO_BUENO) $(libraries) $(lib_prebuilt) $(override_libraries)
	@echo " Helper Linking $(COMPILERNAME) $$@"
	@mkdir -p $$(@D)
ifeq ($(TOOLCHAIN),arm)
	$(Q) $(LD) $$^ $(LFLAGS) --list=$$*.map -o $$@
else
	$(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $$@ $$(call source-to-object,$2) $(LFLAGS)
endif
endef

FILTER_OUT = $(foreach v,$(2),$(if $(findstring $(1),$(v)),,$(v)))
FILTER_IN = $(foreach v,$(2),$(if $(findstring $(1),$(v)),$(v)))
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
