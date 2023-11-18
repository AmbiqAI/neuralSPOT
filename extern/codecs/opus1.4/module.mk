local_src := $(wildcard $(subdirectory)/src/src/*.c)
local_src += $(wildcard $(subdirectory)/src/src/*.s)
local_src += $(wildcard $(subdirectory)/src/celt/*.c)
local_src += $(wildcard $(subdirectory)/src/celt/*.s)
local_src += $(wildcard $(subdirectory)/src/silk/*.c)
local_src += $(wildcard $(subdirectory)/src/silk/float/*.c)
local_src += $(wildcard $(subdirectory)/src/silk/*.s)
local_src += $(wildcard $(subdirectory)/src/*.c)

includes_api += $(subdirectory)/src
includes_api += $(subdirectory)/src/celt
includes_api += $(subdirectory)/src/silk
includes_api += $(subdirectory)/src/silk/float
includes_api += $(subdirectory)/src/src
includes_api += $(subdirectory)/src/include

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)

$(eval $(call make-library, $(local_bin)/opus14.a, $(local_src)))