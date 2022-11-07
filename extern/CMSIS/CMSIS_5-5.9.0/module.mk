
# NN Sources
local_src := $(wildcard $(subdirectory)/CMSIS/NN/Source/ActivationFunctions/*.c)
local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/BasicMathFunctions/*.c)
local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/ConcatenationFunctions/*.c)
local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/ConvolutionFunctions/*.c)
local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/FullyConnectedFunctions/*.c)
local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/NNSupportFunctions/*.c)
local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/PoolingFunctions/*.c)
local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/ReshapeFunctions/*.c)
local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/SVDFunctions/*.c)
local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/SoftmaxFunctions/*.c)


# Base NN
includes_api += $(subdirectory)/CMSIS/NN/Include
includes_api += $(subdirectory)/CMSIS/DSP/Include

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)

$(eval $(call make-library, $(local_bin)/cmsis.a, $(local_src)))