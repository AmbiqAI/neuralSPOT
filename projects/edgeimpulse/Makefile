include make/helpers.mk
include make/neuralspot_config.mk
include make/neuralspot_toolchain.mk
include make/jlink.mk
include make/ns_for_ei.mk

local_app_name := main
PROJECT := ei_yes_no

# The Model

sources := $(wildcard $(PROJECT)/src/*.c)
sources += $(wildcard $(PROJECT)/src/*.cc)
sources += $(wildcard $(PROJECT)/src/*.cpp)
sources += $(wildcard $(PROJECT)/src/*.s)
sources += $(wildcard $(PROJECT)/src/ns-core/*.c)
sources += $(wildcard $(PROJECT)/src/ns-core/*.cc)
sources += $(wildcard $(PROJECT)/src/ns-core/*.cpp)
sources += $(wildcard $(PROJECT)/src/ns-core/*.s)

# EdgeImpulse Stuff

# Common Code
sources += $(wildcard src/ambiq/*.cpp)

# Per-project EI stuff
sources +=   $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/DSP/Source/TransformFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/DSP/Source/CommonTables/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/DSP/Source/BasicMathFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/DSP/Source/ComplexMathFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/DSP/Source/FastMathFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/DSP/Source/SupportFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/DSP/Source/MatrixFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/DSP/Source/StatisticsFunctions/*.c) \
	         $(wildcard $(PROJECT)/src/tflite-model/*.cpp) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/dsp/kissfft/*.cpp) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/dsp/dct/*.cpp) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/dsp/memory.cpp) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/porting/posix/*.c*) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/porting/mingw32/*.c*) \
             $(wildcard $(PROJECT)/src/edge-impulse-sdk/tensorflow/lite/kernels/*.cc) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/tensorflow/lite/kernels/internal/*.cc) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/tensorflow/lite/micro/kernels/*.cc) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/tensorflow/lite/micro/*.cc) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/tensorflow/lite/micro/memory_planner/*.cc) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/tensorflow/lite/core/api/*.cc) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/NN/Source/ActivationFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/NN/Source/BasicMathFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/NN/Source/ConcatenationFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/NN/Source/ConvolutionFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/NN/Source/FullyConnectedFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/NN/Source/NNSupportFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/NN/Source/PoolingFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/NN/Source/ReshapeFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/NN/Source/SoftmaxFunctions/*.c) \
			 $(wildcard $(PROJECT)/src/edge-impulse-sdk/CMSIS/NN/Source/SVDFunctions/*.c)
			            
sources += $(PROJECT)/src/edge-impulse-sdk/tensorflow/lite/c/common.c

VPATH+=$(dir $(sources))

targets  := $(BINDIR)/$(local_app_name).axf
targets  += $(BINDIR)/$(local_app_name).bin

objects      = $(call source-to-object,$(sources))
dependencies = $(subst .o,.d,$(objects))

DEFINES += EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN=1 # Use CMSIS-NN functions in the SDK
DEFINES += __ARM_FEATURE_DSP=1 					  # Enable CMSIS-DSP optimized features
DEFINES += TF_LITE_DISABLE_X86_NEON=1
DEFINES += EI_CLASSIFIER_ALLOCATION_STATIC=1
DEFINES += STACK_SIZE=8192
DEFINES += NS_MALLOC_HEAP_SIZE_IN_K=240

LOCAL_INCLUDES += src
LOCAL_INCLUDES += $(PROJECT)/src
LOCAL_INCLUDES += $(PROJECT)/src/ns-core
LOCAL_INCLUDES += $(PROJECT)/src/edge-impulse-sdk/CMSIS/NN/Include/
LOCAL_INCLUDES += $(PROJECT)/src/edge-impulse-sdk/CMSIS/DSP/PrivateInclude/

CFLAGS     += $(addprefix -D,$(DEFINES))
CFLAGS     += $(addprefix -I $(PROJECT)/includes/,$(INCLUDES))
CFLAGS     += $(addprefix -I ,$(LOCAL_INCLUDES))
LINKER_FILE := $(PROJECT)/libs/linker_script.ld
libraries += $(addprefix $(PROJECT)/,$(libs))

all: $(BINDIR) $(objects) $(targets)

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	@echo "Windows_NT"
	@echo $(Q) $(RM) -rf $(BINDIR)/*
	$(Q) $(RM) -rf $(BINDIR)/*
else
	$(Q) $(RM) -rf $(BINDIR) $(JLINK_CF)
endif

ifneq "$(MAKECMDGOALS)" "clean"
  include $(dependencies)
endif

$(BINDIR):
	@mkdir -p $@

$(BINDIR)/%.o: %.cc
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	@mkdir -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CCFLAGS) $< -o $@

$(BINDIR)/%.o: %.cpp
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	@mkdir -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CCFLAGS) $< -o $@

$(BINDIR)/%.o: %.c
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	@mkdir -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CONLY_FLAGS) $< -o $@

$(BINDIR)/%.o: %.s
	@echo " Assembling $(COMPILERNAME) $<"
	@mkdir -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

$(BINDIR)/$(local_app_name).axf: $(objects)
	@echo " Linking $(COMPILERNAME) $@"
	@mkdir -p $(@D)
	$(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $@ $(objects) $(LFLAGS)

$(BINDIR)/$(local_app_name).bin: $(BINDIR)/$(local_app_name).axf 
	@echo " Copying $(COMPILERNAME) $@..."
	@mkdir -p $(@D)
	$(Q) $(CP) $(CPFLAGS) $< $@
	$(Q) $(OD) $(ODFLAGS) $< > $(BINDIR)/$(local_app_name).lst
	$(Q) $(SIZE) $(objects) $(lib_prebuilt) $< > $(BINDIR)/$(local_app_name).size

$(JLINK_CF):
	@echo " Creating JLink command sequence input file..."
	$(Q) echo "ExitOnError 1" > $@
	$(Q) echo "Reset" >> $@
	$(Q) echo "LoadFile $(BINDIR)/$(local_app_name).bin, $(JLINK_PF_ADDR)" >> $@
	$(Q) echo "Exit" >> $@

.PHONY: deploy
deploy: $(JLINK_CF)
	@echo " Deploying $< to device (ensure JLink USB connected and powered on)..."
	$(Q) $(JLINK) $(JLINK_CMD)

.PHONY: view
view:
	@echo " Printing SWO output (ensure JLink USB connected and powered on)..."
	$(Q) $(JLINK_SWO) $(JLINK_SWO_CMD)

%.d: ;
