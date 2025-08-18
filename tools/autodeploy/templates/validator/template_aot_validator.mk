local_app_name := aot_validator

# -------- Helios‑AOT generated sources --------------------------------------
local_src := $(wildcard $(subdirectory)/../NS_AD_NAME_AOT_aot/NS_AD_NAME_AOT/src/*.c)
local_src += $(wildcard $(subdirectory)/../NS_AD_NAME_AOT_aot/NS_AD_NAME_AOT/src/*.cc)
local_src += $(wildcard $(subdirectory)/../NS_AD_NAME_AOT_aot/NS_AD_NAME_AOT/src/*.cpp)
local_src += $(wildcard $(subdirectory)/../NS_AD_NAME_AOT_aot/NS_AD_NAME_AOT/src/*.s)

# -------- Validator sources -------------------------------------------------
local_src += $(wildcard $(subdirectory)/src/refactor/*.c)
local_src += $(wildcard $(subdirectory)/src/refactor/*.cc)
local_src += $(wildcard $(subdirectory)/src/refactor/*.cpp)
local_src += $(wildcard $(subdirectory)/src/refactor/*.s)
local_src += $(wildcard $(subdirectory)/src/refactor/aot/*.c)
local_src += $(wildcard $(subdirectory)/src/refactor/aot/*.cc)
local_src += $(wildcard $(subdirectory)/src/refactor/aot/*.cpp)
local_src += $(wildcard $(subdirectory)/src/refactor/aot/*.s)

# -------- CMSIS‑NN (needed by AOT kernels) ----------------------------------
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/ActivationFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/BasicMathFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/ConcatenationFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/ConvolutionFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/FullyConnectedFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/LSTMFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/NNSupportFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/PadFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/PoolingFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/QuantizationFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/ReshapeFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/SoftmaxFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/StridedSliceFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/SVDFunctions/*.c)
local_src += $(wildcard $(subdirectory)/../../ns-cmsis-nn/Source/TransposeFunctions/*.c)

local_bin := $(BINDIR)/$(subdirectory)

includes_api += $(subdirectory)/../NS_AD_NAME_AOT_aot/NS_AD_NAME_AOT/includes-api
includes_api += $(subdirectory)/../../ns-cmsis-nn/Include
includes_api += $(subdirectory)/../../ns-cmsis-nn/Include/Internal

bindirs  += $(local_bin)
examples += $(local_bin)/$(local_app_name).axf
examples += $(local_bin)/$(local_app_name).bin

$(eval $(call make-axf, $(local_bin)/$(local_app_name), $(local_src)))
