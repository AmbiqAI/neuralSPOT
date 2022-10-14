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
define make-library
libraries += $1
sources   += $2

$1: $(call source-to-object,$2)
	@echo " Building $(AR) $$@ to make library $$@"
	@mkdir -p $$(@D)
	$(Q) $(AR) rsc $$@ $$^
endef

# $(call make-axf, axf-name, source-file-list, main-file)
define make-axf
local_objs := $(call source-to-object,$1)
$2.axf: $(local_objs) $(libraries)
	@echo " Linking $(COMPILERNAME) $@"
	@mkdir -p $(@D)
	$(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $@ $(LFLAGS)
endef

FILTER_OUT = $(foreach v,$(2),$(if $(findstring $(1),$(v)),,$(v)))
FILTER_IN = $(foreach v,$(2),$(if $(findstring $(1),$(v)),$(v)))