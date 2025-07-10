local_app_name := anomaly_detection
# local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/api/*.cc)
local_src += $(wildcard $(subdirectory)/model/*.cc)
local_src += $(wildcard $(subdirectory)/*.cc)
# local_src += $(wildcard $(subdirectory)/src/*.cpp)
# local_src += $(wildcard $(subdirectory)/src/*.s)
local_bin := $(BINDIR)/$(subdirectory)
includes_api += $(subdirectory)/api

bindirs   += $(local_bin)
# sources   += $(local_src)
examples  += $(local_bin)/$(local_app_name).axf
examples  += $(local_bin)/$(local_app_name).bin
$(eval $(call make-axf, $(local_bin)/$(local_app_name), $(local_src)))
