local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)
includes_api += $(subdirectory)/includes-api

ifeq ($(TOOLCHAIN),arm)
lib_prebuilt += $(subdirectory)/libs/libopus.lib
else
lib_prebuilt += $(subdirectory)/libs/libopus.a
endif

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)
# sources   += $(local_src)
$(eval $(call make-library, $(local_bin)/codecs.a, $(local_src)))