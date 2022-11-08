
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
includes_api += $(subdirectory)/CMSIS/Core/Include
includes_api += $(subdirectory)/CMSIS/NN/Include
includes_api += $(subdirectory)/CMSIS/DSP/Include
includes_api += $(subdirectory)/CMSIS/DSP/PrivateInclude

lib_prebuilt += $(subdirectory)/lib/libCMSISDSP.a
lib_prebuilt += $(subdirectory)/lib/libcmsis-nn.a

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)

# $(eval $(call make-library, $(local_bin)/cmsis.a, $(local_src)))