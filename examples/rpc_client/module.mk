local_app_name := rpc_client
local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)
local_bin := $(BINDIR)/$(subdirectory)

bindirs   += $(local_bin)
# sources   += $(local_src)
examples  += $(local_bin)/$(local_app_name).axf
examples  += $(local_bin)/$(local_app_name).bin
# mains     += $(local_bin)/src/$(local_app_name).o
$(eval $(call make-axf, $(local_bin)/$(local_app_name), $(local_src)))
