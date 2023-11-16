
##### Toolchain Defaults #####
TOOLCHAIN ?= arm-none-eabi
ifeq ($(TOOLCHAIN),arm-none-eabi)
COMPILERNAME := gcc
else ifeq ($(TOOLCHAIN),arm)
COMPILERNAME := clang
endif

BINDIR := build
NESTDIR := nest
SHELL  :=/bin/bash

##### Target Hardware Defaults #####
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



##### Extern Library Defaults #####
ifndef AS_VERSION
AS_VERSION := R4.4.1
endif
ifndef TF_VERSION
TF_VERSION := 0264234_Nov_15_2023
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

##### AmbiqSuite Config and HW Feature Control Flags #####
ifneq ($(BRD),apollo4l)
	DEFINES+= AM_HAL_TEMPCO_LP
	DEFINES+= NS_AUDADC_PRESENT
	ifneq ($(EVB),blue_kxr_evb)
		DEFINES+= NS_PDM1TO3_PRESENT
	endif
	DEFINES+= NS_USB1_PRESENT
	USB_PRESENT := 1
else
	USB_PRESENT := 0
endif

ifeq ($(EVB),blue_evb)
	BLE_PRESENT := 1
else ifeq ($(EVB),blue_kbr_evb)
	BLE_PRESENT := 1
else ifeq ($(EVB),blue_kxr_evb)
	BLE_PRESENT := 1
else
	BLE_PRESENT := 0
endif

# application stack and heap size
STACK_SIZE_IN_32B_WORDS ?= 4096
NS_MALLOC_HEAP_SIZE_IN_K ?= 24

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
else
	BLE_SUPPORTED := 0
endif

DEFINES+= SEC_ECC_CFG=SEC_ECC_CFG_HCI
# DEFINES+= WSF_TRACE_ENABLED
# DEFINES+= HCI_TRACE_ENABLED
DEFINES+= AM_DEBUG_PRINTF

##### Common AI Precompiler Directives #####
# 1 = load TF library with debug info, turn on TF debug statements
MLDEBUG ?= 0

ifeq ($(TF_VERSION),d5f819d_Aug_10_2023)
	DEFINES+= NS_TFSTRUCTURE_RECENT
endif
ifeq ($(TF_VERSION),0264234_Nov_15_2023)
	DEFINES+= NS_TFSTRUCTURE_RECENT
endif
ifeq ($(TF_VERSION),fecdd5d)
	DEFINES+= NS_TFSTRUCTURE_RECENT
endif


# 1 = load optimized TF library with prints enabled, turn on TF profiler
MLPROFILE := 0
TFLM_VALIDATOR := 0
TFLM_VALIDATOR_MAX_EVENTS := 40
# AUDIO_DEBUG := 0    # 1 = link in RTT, dump audio to RTT console
# ENERGY_MODE := 0    # 1 = enable energy measurements via UART1
