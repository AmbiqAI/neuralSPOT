local_app_name := tflm_validator
local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
# local_src += $(wildcard $(subdirectory)/ns_model/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)
local_bin := $(BINDIR)/$(subdirectory)
# includes_api += $(subdirectory)/ns_model_includes

bindirs   += $(local_bin)
sources   += $(local_src)
examples  += $(local_bin)/$(local_app_name).axf
examples  += $(local_bin)/$(local_app_name).bin
mains     += $(local_bin)/src/$(local_app_name).o
