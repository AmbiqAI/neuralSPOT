include make/helpers.mk
include make/neuralspot_config.mk
include make/neuralspot_toolchain.mk
include make/jlink.mk
include autogen.mk

local_app_name := edgeimpulse
sources := $(wildcard src/*.c)
sources += $(wildcard src/*.cc)
sources += $(wildcard src/*.cpp)
sources += $(wildcard src/*.s)

# EdgeImpulse Stuff
sources +=   $(wildcard src/edge-impulse-sdk/porting/ambiq/*.cpp) \
 			 $(wildcard src/edge-impulse-sdk/CMSIS/DSP/Source/TransformFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/DSP/Source/CommonTables/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/DSP/Source/BasicMathFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/DSP/Source/ComplexMathFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/DSP/Source/FastMathFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/DSP/Source/SupportFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/DSP/Source/MatrixFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/DSP/Source/StatisticsFunctions/*.c) \
	         $(wildcard src/tflite-model/*.cpp) \
			 $(wildcard src/edge-impulse-sdk/dsp/kissfft/*.cpp) \
			 $(wildcard src/edge-impulse-sdk/dsp/dct/*.cpp) \
			 $(wildcard src/edge-impulse-sdk/dsp/memory.cpp) \
			 $(wildcard src/edge-impulse-sdk/porting/posix/*.c*) \
			 $(wildcard src/edge-impulse-sdk/porting/mingw32/*.c*) \
             $(wildcard src/edge-impulse-sdk/tensorflow/lite/kernels/*.cc) \
			 $(wildcard src/edge-impulse-sdk/tensorflow/lite/kernels/internal/*.cc) \
			 $(wildcard src/edge-impulse-sdk/tensorflow/lite/micro/kernels/*.cc) \
			 $(wildcard src/edge-impulse-sdk/tensorflow/lite/micro/*.cc) \
			 $(wildcard src/edge-impulse-sdk/tensorflow/lite/micro/memory_planner/*.cc) \
			 $(wildcard src/edge-impulse-sdk/tensorflow/lite/core/api/*.cc) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/NN/Source/ActivationFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/NN/Source/BasicMathFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/NN/Source/ConcatenationFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/NN/Source/ConvolutionFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/NN/Source/FullyConnectedFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/NN/Source/NNSupportFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/NN/Source/PoolingFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/NN/Source/ReshapeFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/NN/Source/SoftmaxFunctions/*.c) \
			 $(wildcard src/edge-impulse-sdk/CMSIS/NN/Source/SVDFunctions/*.c)
			            
sources += src/edge-impulse-sdk/tensorflow/lite/c/common.c

VPATH+=$(dir $(sources))

targets  := $(BINDIR)/$(local_app_name).axf
targets  += $(BINDIR)/$(local_app_name).bin

objects      = $(call source-to-object,$(sources))
dependencies = $(subst .o,.d,$(objects))

DEFINES += EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN=1 # Use CMSIS-NN functions in the SDK
DEFINES += __ARM_FEATURE_DSP=1 					  # Enable CMSIS-DSP optimized features
DEFINES += TF_LITE_DISABLE_X86_NEON=1
DEFINES += EI_CLASSIFIER_ALLOCATION_STATIC=1

LOCAL_INCLUDES += src
LOCAL_INCLUDES += src/edge-impulse-sdk/CMSIS/NN/Include/
LOCAL_INCLUDES += src/edge-impulse-sdk/CMSIS/DSP/PrivateInclude/

CFLAGS     += $(addprefix -D,$(DEFINES))
CFLAGS     += $(addprefix -I includes/,$(INCLUDES))
CFLAGS     += $(addprefix -I ,$(LOCAL_INCLUDES))
LINKER_FILE := libs/linker_script.ld

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
	$(Q) $(OD) $(ODFLAGS) $< > $*.lst
	$(Q) $(SIZE) $(objects) $(lib_prebuilt) $< > $*.size

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
