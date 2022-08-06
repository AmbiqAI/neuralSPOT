local_src := $(wildcard $(subdirectory)/src/*.c)
includes_api += $(subdirectory)/includes-api

local_bin := $(subdirectory)/$(BINDIR)
bindirs   += $(local_bin)
$(eval $(call make-library, $(local_bin)/ns-peripherals.a, $(local_src)))