# $(call source-to-object, source-file-list)
source-to-object = $(subst src,$(BINDIR),$(subst .c,.o,$(filter %.c,$1))) \
                   $(subst src,$(BINDIR),$(subst .s,.o,$(filter %.s,$1))) \
                   $(subst src,$(BINDIR),$(subst .cc,.o,$(filter %.cc,$1))) \
                   $(subst src,$(BINDIR),$(subst .cpp,.o,$(filter %.cpp,$1))) \

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