local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)
includes_api += $(subdirectory)/includes-api

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)

$(eval $(call make-library, $(local_bin)/erpc.a, $(local_src)))