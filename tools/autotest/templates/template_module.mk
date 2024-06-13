local_app_name := NS_AD_NAME
local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)
local_src := $(wildcard $(subdirectory)/src/unity/*.c)
local_src += $(wildcard $(subdirectory)/src/unity/*.cc)
local_src += $(wildcard $(subdirectory)/src/unity/*.cpp)
local_src += $(wildcard $(subdirectory)/src/unity/*.s)
local_bin := $(BINDIR)/$(subdirectory)

bindirs   += $(local_bin)
examples  += $(local_bin)/$(local_app_name).axf
examples  += $(local_bin)/$(local_app_name).bin
$(eval $(call make-axf, $(local_bin)/$(local_app_name), $(local_src)))
