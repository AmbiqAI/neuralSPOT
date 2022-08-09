local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.s)
includes_api += $(subdirectory)/includes-api

local_bin := $(subdirectory)/$(BINDIR)
bindirs   += $(local_bin)
$(eval $(call make-library, $(local_bin)/ns-harness.a, $(local_src)))