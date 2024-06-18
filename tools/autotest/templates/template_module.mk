local_app_name := NS_AT_MAIN
local_src := $(wildcard $(subdirectory)/*.c)
local_src += $(wildcard $(subdirectory)/*.cc)
local_src += $(wildcard $(subdirectory)/*.cpp)
local_src += $(wildcard $(subdirectory)/*.s)
local_src += $(wildcard $(subdirectory)/unity/*.c)
local_src += $(wildcard $(subdirectory)/unity/*.cc)
local_src += $(wildcard $(subdirectory)/unity/*.cpp)
local_src += $(wildcard $(subdirectory)/unity/*.s)
local_bin := $(BINDIR)/$(subdirectory)

bindirs   += $(local_bin)
examples  += $(local_bin)/$(local_app_name).axf
examples  += $(local_bin)/$(local_app_name).bin
$(eval $(call make-axf, $(local_bin)/$(local_app_name), $(local_src)))
