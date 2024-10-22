local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/arducam/*.c)
local_src += $(wildcard $(subdirectory)/src/arducam/*.cc)
local_src += $(wildcard $(subdirectory)/src/arducam/*.cpp)
local_src += $(wildcard $(subdirectory)/src/jpeg-decoder/*.c)
local_src += $(wildcard $(subdirectory)/src/jpeg-decoder/*.cc)
local_src += $(wildcard $(subdirectory)/src/jpeg-decoder/*.cpp)
includes_api += $(subdirectory)/includes-api

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)
$(eval $(call make-library, $(local_bin)/ns-camera.a, $(local_src)))
