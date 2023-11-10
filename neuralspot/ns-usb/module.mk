# This is so stupid - make sure this happens first
local_src := $(wildcard $(subdirectory)/src/overrides/*.c)
local_src += $(wildcard $(subdirectory)/src/overrides/*.cc)
local_src += $(wildcard $(subdirectory)/src/overrides/*.s)
includes_api += $(subdirectory)/includes-api

local_bin := $(BINDIR)/$(subdirectory)
# bindirs   += $(local_bin)
$(eval $(call make-override-library, $(local_bin)/ns-usb-overrides.a, $(local_src)))

# Now the real lib
local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.s)
includes_api += $(subdirectory)/includes-api

# local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)
$(eval $(call make-library, $(local_bin)/ns-usb.a, $(local_src)))
