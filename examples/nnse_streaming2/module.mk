local_app_name := nnse_streaming2

local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)

TFLM_BUILD := 1# 1 : TFLM, 0 : NNSP
MODEL_RNN := 0# 1 : RNN, 0 : UNET

ifeq ($(strip $(TFLM_BUILD)),1)
	ifeq ($(strip $(MODEL_RNN)),1) # RNN
		local_src += $(wildcard $(subdirectory)/src/tflm_rnn/*.c)
		local_src += $(wildcard $(subdirectory)/src/tflm_rnn/*.cc)
		local_src += $(wildcard $(subdirectory)/src/tflm_rnn/*.cpp)
		local_src += $(wildcard $(subdirectory)/src/tflm_rnn/*.s)
	else
		local_src += $(wildcard $(subdirectory)/src/tflm_unet/*.c)
		local_src += $(wildcard $(subdirectory)/src/tflm_unet/*.cc)
		local_src += $(wildcard $(subdirectory)/src/tflm_unet/*.cpp)
		local_src += $(wildcard $(subdirectory)/src/tflm_unet/*.s)
	endif
else
	ifeq ($(strip $(MODEL_RNN)),1) # RNN
		local_src += $(wildcard $(subdirectory)/src/nnsp_rnn/*.c)
		local_src += $(wildcard $(subdirectory)/src/nnsp_rnn/*.cc)
		local_src += $(wildcard $(subdirectory)/src/nnsp_rnn/*.cpp)
		local_src += $(wildcard $(subdirectory)/src/nnsp_rnn/*.s)	
	else
$(error "Invalid configuration: UNET is NOT supported when you use NNSP!")
	endif
endif

local_bin := $(BINDIR)/$(subdirectory)

bindirs   += $(local_bin)
# sources   += $(local_src)
examples  += $(local_bin)/$(local_app_name).axf
examples  += $(local_bin)/$(local_app_name).bin
# mains     += $(local_bin)/src/$(local_app_name).o
$(eval $(call make-axf, $(local_bin)/$(local_app_name), $(local_src)))



