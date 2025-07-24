local_app_name := NS_AD_NAME
local_src := $(wildcard $(subdirectory)/../NS_AD_NAME_AOT_aot/NS_AD_NAME_AOT/src/*.c)
local_src += $(wildcard $(subdirectory)/../NS_AD_NAME_AOT_aot/NS_AD_NAME_AOT/src/*.cc)
local_src += $(wildcard $(subdirectory)/../NS_AD_NAME_AOT_aot/NS_AD_NAME_AOT/src/*.cpp)
local_src += $(wildcard $(subdirectory)/../NS_AD_NAME_AOT_aot/NS_AD_NAME_AOT/src/*.s)
local_src += $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)
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

bindirs   += $(local_bin)
examples  += $(local_bin)/$(local_app_name).axf
examples  += $(local_bin)/$(local_app_name).bin

$(eval $(call make-axf, $(local_bin)/$(local_app_name), $(local_src)))
