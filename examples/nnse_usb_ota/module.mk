local_app_name := nnse_usb_ota
local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)
local_bin := $(BINDIR)/$(subdirectory)

bindirs   += $(local_bin)
examples  += $(local_bin)/$(local_app_name).axf
examples  += $(local_bin)/$(local_app_name).bin
# # mains 	+= $(wildcard $(subdirectory)/src/*.o)
# # mains     += $(local_bin)/src/$(local_app_name).o
# mains     += $(wildcard  $(local_bin)/src/*.o)
$(eval $(call make-axf, $(local_bin)/$(local_app_name), $(local_src)))

includes_api += $(PROJECT_ROOT)/extern/tensorflow/ns_tflm_v1_0_0/third_party/flatbuffers/include
