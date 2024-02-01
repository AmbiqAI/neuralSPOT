local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)
includes_api += $(subdirectory)/includes-api

# Diffferentiate between apollo3 and apollo4
ifeq ($(BOARD),apollo3p)
  local_src += $(wildcard $(subdirectory)/src/apollo3/*.c)
  local_src += $(wildcard $(subdirectory)/src/apollo3/*.cc)
  local_src += $(wildcard $(subdirectory)/src/apollo3/*.cpp)
  local_src += $(wildcard $(subdirectory)/src/apollo3/*.s)
  includes_api += $(subdirectory)/includes-api/apollo3
else
  local_src += $(wildcard $(subdirectory)/src/apollo4/*.c)
  local_src += $(wildcard $(subdirectory)/src/apollo4/*.cc)
  local_src += $(wildcard $(subdirectory)/src/apollo4/*.cpp)
  local_src += $(wildcard $(subdirectory)/src/apollo4/*.s)
  includes_api += $(subdirectory)/includes-api/apollo4
endif

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)
$(eval $(call make-library, $(local_bin)/ns-utils.a, $(local_src)))
