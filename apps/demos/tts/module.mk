local_app_name := tts
local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/phone2fuse/*.c)
local_src += $(wildcard $(subdirectory)/src/phone2fuse/*.cc)
local_src += $(wildcard $(subdirectory)/src/fuse2mel/*.c)
local_src += $(wildcard $(subdirectory)/src/fuse2mel/*.cc)

local_bin := $(BINDIR)/$(subdirectory)

bindirs   += $(local_bin)
examples  += $(local_bin)/$(local_app_name).axf
examples  += $(local_bin)/$(local_app_name).bin
$(eval $(call make-axf, $(local_bin)/$(local_app_name), $(local_src)))

local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)
# If validator sources are in src/tflm_validator/src/, uncomment these:
# local_src += $(wildcard $(subdirectory)/src/tflm_validator/src/*.c)
# local_src += $(wildcard $(subdirectory)/src/tflm_validator/src/*.cc)
# local_src += $(wildcard $(subdirectory)/src/tflm_validator/src/*.cpp)
# local_src += $(wildcard $(subdirectory)/src/tflm_validator/src/*.s)

includes_api += apps/demos/tts/src
includes_api += apps/demos/tts/src/phone2fuse
includes_api += apps/demos/tts/src/fuse2mel
