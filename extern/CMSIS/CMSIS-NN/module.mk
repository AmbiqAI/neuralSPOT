
# NN Sources
# local_src := $(wildcard $(subdirectory)/CMSIS/NN/Source/ActivationFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/BasicMathFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/ConcatenationFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/ConvolutionFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/FullyConnectedFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/NNSupportFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/PoolingFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/ReshapeFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/SVDFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/NN/Source/SoftmaxFunctions/*.c)


$(info Including CMSIS-NN)
local_src := $(wildcard $(subdirectory)/Source/ActivationFunctions/*.c)
local_src += $(wildcard $(subdirectory)/Source/BasicMathFunctions/*.c)
local_src += $(wildcard $(subdirectory)/Source/ConcatenationFunctions/*.c)
local_src += $(wildcard $(subdirectory)/Source/ConvolutionFunctions/*.c)
local_src += $(wildcard $(subdirectory)/Source/FullyConnectedFunctions/*.c)
local_src += $(wildcard $(subdirectory)/Source/LSTMFunctions/*.c)
local_src += $(wildcard $(subdirectory)/Source/NNSupportFunctions/*.c)
local_src += $(wildcard $(subdirectory)/Source/PadFunctions/*.c)
local_src += $(wildcard $(subdirectory)/Source/PoolingFunctions/*.c)
local_src += $(wildcard $(subdirectory)/Source/ReshapeFunctions/*.c)
local_src += $(wildcard $(subdirectory)/Source/SVDFunctions/*.c)
local_src += $(wildcard $(subdirectory)/Source/SoftmaxFunctions/*.c)
local_src += $(wildcard $(subdirectory)/Source/TransposeFunctions/*.c)

# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/BasicMathFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/BayesFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/CommonTables/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/ComplexMathFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/ControllerFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/DistanceFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/FastMathFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/FilteringFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/InterpolationFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/MatrixFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/QuaternionMathFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/SVMFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/StatisticsFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/SupportFunctions/*.c)
# local_src += $(wildcard $(subdirectory)/CMSIS/DSP/Source/TransformFunctions/*.c)

# Includes
# includes_api += $(subdirectory)/CoreInclude
# includes_api += $(subdirectory)/CMSIS/NN/Include
includes_api += $(subdirectory)/Include
# includes_api += $(subdirectory)/PrivateInclude

# ifeq ($(ARCH),apollo5)
# 	CMSISP := m55
# else
# 	CMSISP := m4
# endif

# ifeq ($(TOOLCHAIN),arm)
# lib_prebuilt += $(subdirectory)/lib/libCMSISDSP-$(CMSISP)-armclang.a
# else
# ifeq ($(GCC13_EXPERIMENTAL),1)
# lib_prebuilt += $(subdirectory)/lib/libCMSISDSP-$(CMSISP)-gcc13.a
# else
# lib_prebuilt += $(subdirectory)/lib/libCMSISDSP-$(CMSISP)-gcc.a
# endif
# endif
# lib_prebuilt += $(subdirectory)/lib/libCMSISDSP.a
source += $(local_src)
# local_bin := $(BINDIR)/$(subdirectory)
# bindirs   += $(local_bin)

# $(eval $(call make-library, $(local_bin)/cmsis.a, $(local_src)))