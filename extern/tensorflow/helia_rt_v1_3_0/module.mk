# Include paths
includes_api += $(subdirectory)/.
includes_api += $(subdirectory)/third_party
includes_api += $(subdirectory)/third_party/flatbuffers/include
includes_api += $(subdirectory)/third_party/gemmlowp

# Preprocessor defines
DEFINES += NS_TFSTRUCTURE_RECENT
DEFINES += NS_TFLM_NEW_MICRO_PROFILER

# Determine short architecture tag
TFP := $(if $(filter apollo5,$(ARCH)),cm55,cm4)

# Determine build type suffix
ifeq ($(MLDEBUG),1)
  BUILD_TYPE := debug
else ifeq ($(MLPROFILE),1)
  BUILD_TYPE := release-with-logs
else
  BUILD_TYPE := release
endif

# Determine toolchain name (neuralSPOT uses arm instead of armclang)
TOOLCHAIN_NAME := $(if $(filter arm,$(TOOLCHAIN)),armclang,gcc)

# Construct final static library path
lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-$(TFP)-$(TOOLCHAIN_NAME)-$(BUILD_TYPE).a
