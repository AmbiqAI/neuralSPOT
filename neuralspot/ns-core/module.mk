local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)

ifeq ($(TOOLCHAIN),arm)
local_src += $(wildcard $(subdirectory)/src/armclang/*.c)
local_src += $(wildcard $(subdirectory)/src/armclang/*.cc)
local_src += $(wildcard $(subdirectory)/src/armclang/*.cpp)
local_src += $(wildcard $(subdirectory)/src/armclang/*.s)
else
local_src += $(wildcard $(subdirectory)/src/gcc/*.c)
local_src += $(wildcard $(subdirectory)/src/gcc/*.cc)
local_src += $(wildcard $(subdirectory)/src/gcc/*.cpp)
local_src += $(wildcard $(subdirectory)/src/gcc/*.s)
endif

# sources   += $(local_src)

includes_api += $(subdirectory)/includes-api

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)
$(eval $(call make-library, $(local_bin)/ns-core.a, $(local_src)))
