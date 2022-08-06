# $(call source-to-object, source-file-list)
source-to-object = $(subst .c,.o,$(filter %.c,$1)) \
                   $(subst .s,.o,$(filter %.s,$1)) \
                   $(subst .cc,.o,$(filter %.cc,$1))

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