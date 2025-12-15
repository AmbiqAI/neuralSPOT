local_src := $(wildcard $(subdirectory)/src/src/*.c)
local_src += $(wildcard $(subdirectory)/src/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/src/*.s)
local_src += $(wildcard $(subdirectory)/src/celt/*.c)
local_src += $(wildcard $(subdirectory)/src/celt/*.cc)
local_src += $(wildcard $(subdirectory)/src/celt/*.cpp)
local_src += $(wildcard $(subdirectory)/src/celt/*.s)
includes_api += $(subdirectory)/src/src
includes_api += $(subdirectory)/src/celt
includes_api += $(subdirectory)/src/include

# ifeq ($(TOOLCHAIN),arm)
# lib_prebuilt += $(subdirectory)/libs/libopus.lib
# else
# lib_prebuilt += $(subdirectory)/libs/libopus.a
# endif

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)
# sources   += $(local_src)
$(eval $(call make-library, $(local_bin)/octopus.a, $(local_src)))