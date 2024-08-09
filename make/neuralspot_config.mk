
##### Toolchain Defaults #####
TOOLCHAIN ?= arm-none-eabi
ifeq ($(TOOLCHAIN),arm-none-eabi)
COMPILERNAME := gcc
else ifeq ($(TOOLCHAIN),arm)
COMPILERNAME := clang
else ifeq ($(TOOLCHAIN),llvm)
COMPILERNAME := clang
endif

NESTDIR := nest
SHELL  :=/bin/bash

ifndef PLATFORM
PLATFORM := apollo4p_evb
endif

##### Target Hardware Defaults #####
# PLATFORM defines the MCU (aka BOARD) and the EVB (aka EVB)
# in the form of BOARD_EVB, where board e.g. is apollo4p or apollo4l
# and EVB is evb, blue_evb, blue_kbr_evb, or blue_kxr_evb, etc
# Get the first word of the PLATFORM, which is the MCU
BOARD := $(firstword $(subst _, ,$(PLATFORM)))

# EVB is the rest of the PLATFORM, which is the EVB
EVB := $(wordlist 2,$(words $(subst _, ,$(PLATFORM))),$(subst _, ,$(PLATFORM)))

# Replace spaces with underscores
space := $(null) #
EVB := $(subst $(space),_,$(EVB))

# Set the ARCH to apollo3, apollo4, or apollo5
ifeq ($(findstring apollo3,$(BOARD)),apollo3)
ARCH := apollo3
else ifeq ($(findstring apollo4,$(BOARD)),apollo4)
ARCH := apollo4
else ifeq ($(findstring apollo5,$(BOARD)),apollo5)
ARCH := apollo5
endif

# $(info BOARD: $(BOARD))
# $(info EVB: $(EVB))
# $(info ARCH: $(ARCH))

ifndef BOARD
BOARD  :=apollo4p
endif
BRD   :=$(BOARD)
BOARDROOT = $(firstword $(subst _, ,$(BRD)))
ifndef EVB
EVB    :=evb
endif
PART   = $(BOARDROOT)
CPU    = cortex-m4
FPU    = fpv4-sp-d16
# Default to FPU hardware calling convention.  However, some customers and/or
# applications may need the software calling convention.
#FABI     = softfp
FABI     = hard

BINDIRROOT := build
BINDIR := $(BINDIRROOT)/$(BOARDROOT)_$(EVB)/$(TOOLCHAIN)


##### Extern Library Defaults #####
ifndef AS_VERSION
AS_VERSION := R4.5.0
endif
ifndef TF_VERSION
TF_VERSION := ce72f7b8_Feb_17_2024
endif
SR_VERSION := R7.70a
ERPC_VERSION := R1.9.1
CMSIS_VERSION := CMSIS_5-5.9.0

##### Application Defaults #####
# default target for binary-specific operations such as 'deploy'
EXAMPLE     := all
TARGET      := basic_tf_stub
NESTCOMP    := extern/AmbiqSuite
NESTEGG := basic_tf_stub
NESTSOURCEDIR := examples/$(NESTEGG)/src
TARGETS := apollo4p_evb apollo4p_blue_kbr_evb apollo4p_blue_kxr_evb apollo4l_evb apollo4l_blue_evb

##### AmbiqSuite Config and HW Feature Control Flags #####
# AM_HAL_TEMPCO_LP is only supported by Apollo4
# NS_AUDADC_PRESENT is only supported by Apollo4p and Apollo5
# USB_PRESENT is only supported by Apollo4p and Apollo5
# NS_PDM1TO3_PRESENT is only supported by non-BLE Apollo4p
# NS_BLE_SUPPORTED is support by EVBs with 'blue' in name

# Set BLE_PRESENT
ifeq ($(EVB),blue_evb)
	BLE_PRESENT := 1
else ifeq ($(EVB),blue_kbr_evb)
	BLE_PRESENT := 1
else ifeq ($(EVB),blue_kxr_evb)
	BLE_PRESENT := 1
else ifeq ($(ARCH),apollo3)
	BLE_PRESENT := 1
else
	BLE_PRESENT := 0
endif

# $(info BLE_PRESENT: $(BLE_PRESENT))

# ifeq ($(BLE_PRESENT),1)
# 	ifeq ($(AS_VERSION),R4.3.0)
# 		DEFINES+= NS_BLE_SUPPORTED
# 	else ifeq ($(AS_VERSION),R4.4.1)
# 		DEFINES+= NS_BLE_SUPPORTED
# 	else ifeq ($(AS_VERSION),R4.5.0)
# 		DEFINES+= NS_BLE_SUPPORTED
# 	else ifeq ($(AS_VERSION),R3.1.1)
# 		DEFINES+= NS_BLE_SUPPORTED
# 	endif
# endif

# $(info DEFINES: $(DEFINES))

# Set USB
ifeq ($(PART),apollo4p)
	USB_PRESENT := 1
else ifeq ($(PART),apollo5)
	USB_PRESENT := 1
else
	USB_PRESENT := 0
endif

ifeq ($(USB_PRESENT),1)
	DEFINES+= NS_USB_PRESENT
endif

# Set NS_AUDADC_PRESENT and AM_HAL_TEMPCO_LP
ifeq ($(PART),apollo4p)
	DEFINES+= NS_AUDADC_PRESENT
	DEFINES+= AM_HAL_TEMPCO_LP
else ifeq ($(PART),apollo5)
	DEFINES+= NS_AUDADC_PRESENT
endif

# Set NS_PDM1TO3_PRESENT
ifeq ($(PART),apollo4p)
	ifeq ($(EVB),evb)
		DEFINES+= NS_PDM1TO3_PRESENT
	endif
endif


# application stack and heap size
ifndef STACK_SIZE_IN_32B_WORDS
STACK_SIZE_IN_32B_WORDS ?= 4096
endif

# If LEGACY MALLOC is 1, a heap of NS_MALLOC_HEAP_SIZE_IN_K
# will be allocated for all NS examples. If LEGACY MALLOC is 0,
# the heap must be allocated by the example, but can differ
# per each example

ifeq ($(LEGACY_MALLOC),1)
	DEFINES+= configAPPLICATION_ALLOCATED_HEAP=0
else
	DEFINES+= configAPPLICATION_ALLOCATED_HEAP=1
endif

ifndef NS_MALLOC_HEAP_SIZE_IN_K
NS_MALLOC_HEAP_SIZE_IN_K ?= 32
endif

##### TinyUSB Default Config #####
DEFINES+= CFG_TUSB_MCU=OPT_MCU_APOLLO4
# DEFINES+= BOARD_DEVICE_RHPORT_SPEED=OPT_MODE_HIGH_SPEED

##### BLE Defines
## BLE is only supported by neuralSPOT for AmbiqSuite R4.3.0 and later
ifeq ($(AS_VERSION),R4.3.0)
	BLE_SUPPORTED := $(BLE_PRESENT)
	ifeq ($(BLE_SUPPORTED),1)
		DEFINES+= NS_BLE_SUPPORTED
	endif
else ifeq ($(AS_VERSION),R4.4.1)
	BLE_SUPPORTED := $(BLE_PRESENT)
	ifeq ($(BLE_SUPPORTED),1)
		DEFINES+= NS_BLE_SUPPORTED
	endif
else ifeq ($(AS_VERSION),R4.5.0)
	BLE_SUPPORTED := $(BLE_PRESENT)
	ifeq ($(BLE_SUPPORTED),1)
		DEFINES+= NS_BLE_SUPPORTED
	endif
else ifeq ($(AS_VERSION),R3.1.1)
	BLE_SUPPORTED := $(BLE_PRESENT)
	ifeq ($(BLE_SUPPORTED),1)
		DEFINES+= NS_BLE_SUPPORTED
	endif
else
	BLE_SUPPORTED := 0
endif

# $(info BLE_SUPPORTED: $(BLE_SUPPORTED))
# $(info DEFINES: $(DEFINES))


DEFINES+= SEC_ECC_CFG=SEC_ECC_CFG_HCI
# DEFINES+= WSF_TRACE_ENABLED
# DEFINES+= HCI_TRACE_ENABLED
DEFINES+= AM_DEBUG_PRINTF

##### Common AI Precompiler Directives #####
# 1 = load TF library with debug info, turn on TF debug statements
MLDEBUG ?= 0

# Legacy - dont define for older TFs
DEFINES+= NS_TFSTRUCTURE_RECENT

# ifeq ($(TF_VERSION),d5f819d_Aug_10_2023)
# 	DEFINES+= NS_TFSTRUCTURE_RECENT
# endif
# ifeq ($(TF_VERSION),0264234_Nov_15_2023)
# 	DEFINES+= NS_TFSTRUCTURE_RECENT
# endif
# ifeq ($(TF_VERSION),fecdd5d)
# 	DEFINES+= NS_TFSTRUCTURE_RECENT
# endif
# ifeq ($(TF_VERSION),ce72f7b8_Feb_17_2024)
# 	DEFINES+= NS_TFSTRUCTURE_RECENT
# endif


# 1 = load optimized TF library with prints enabled, turn on TF profiler
MLPROFILE := 0
TFLM_VALIDATOR := 0
TFLM_VALIDATOR_MAX_EVENTS := 40
# AUDIO_DEBUG := 0    # 1 = link in RTT, dump audio to RTT console
# ENERGY_MODE := 0    # 1 = enable energy measurements via UART1

DEFINES+= OPUS_ARM_INLINE_ASM
# DEFINES+= ARM_MATH_CM4
DEFINES+= OPUS_ARM_ASM
