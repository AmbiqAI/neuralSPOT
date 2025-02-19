local_src := $(wildcard $(subdirectory)/src/*.c)
local_src := $(wildcard $(subdirectory)/src/*.cc)
includes_api += $(subdirectory)/includes-api

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)
$(eval $(call make-library, $(local_bin)/ns-model.a, $(local_src)))
