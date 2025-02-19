local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)

ifeq ($(TOOLCHAIN),arm)
local_src += $(wildcard $(subdirectory)/src/$(BOARD)/armclang/*.c)
local_src += $(wildcard $(subdirectory)/src/$(BOARD)/armclang/*.cc)
local_src += $(wildcard $(subdirectory)/src/$(BOARD)/armclang/*.cpp)
local_src += $(wildcard $(subdirectory)/src/$(BOARD)/armclang/*.s)
else ifeq ($(TOOLCHAIN),arm-none-eabi)
local_src += $(wildcard $(subdirectory)/src/$(BOARD)/gcc/*.c)
local_src += $(wildcard $(subdirectory)/src/$(BOARD)/gcc/*.cc)
local_src += $(wildcard $(subdirectory)/src/$(BOARD)/gcc/*.cpp)
local_src += $(wildcard $(subdirectory)/src/$(BOARD)/gcc/*.s)
else
local_src += $(wildcard $(subdirectory)/src/$(BOARD)/llvm/*.c)
local_src += $(wildcard $(subdirectory)/src/$(BOARD)/llvm/*.cc)
local_src += $(wildcard $(subdirectory)/src/$(BOARD)/llvm/*.cpp)
local_src += $(wildcard $(subdirectory)/src/$(BOARD)/llvm/*.s)
endif

# sources   += $(local_src)

includes_api += $(subdirectory)/includes-api

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)
$(eval $(call make-library, $(local_bin)/ns-core.a, $(local_src)))
