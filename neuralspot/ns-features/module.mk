local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)
local_src += $(wildcard $(subdirectory)/src/$(ARCH)/*.c)
local_src += $(wildcard $(subdirectory)/src/$(ARCH)/*.cc)
local_src += $(wildcard $(subdirectory)/src/$(ARCH)/*.cpp)
local_src += $(wildcard $(subdirectory)/src/$(ARCH)/*.s)
includes_api += $(subdirectory)/includes-api
includes_api += $(subdirectory)/$(ARCH)/includes-api

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)
$(eval $(call make-library, $(local_bin)/ns-features.a, $(local_src)))
